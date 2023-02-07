//====================================================================
//============================= Librarys =============================
//====================================================================
#include <avr/pgmspace.h>
//#include <TSYS01.h>
#include <MS5837.h>
#include <cyclopSensor.h>
#include <Wire.h>
#include <SPI.h>
#include <SDI12.h>

#include <Ethernet.h>
#include <EthernetUdp.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

// Serial configuration
#define BAUDRATE 115200

// Cyclops Sensors
// Plateado
#define cdom_type "cdom"
#define cdom_id '1'
#define cdom_read A1
#define cdom_x10 5
#define cdom_x100 6
#define cdom_maxPPB 1500

// Negro
#define phy_type "phy"
#define phy_id '2'
#define phy_read A0
#define phy_x10 7
#define phy_x100 8
#define phy_maxPPB 750

// Amarillo
#define chl_type "chl"
#define chl_id '3'
#define chl_read A2
#define chl_x10 2
#define chl_x100 3
#define chl_maxPPB 500

// SDI-12. Sensor Conductividad
#define SDI_DATA_PIN 9
#define SDI_POWER_PIN 4
#define CE_ADDRESS 0

// UDP configs
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
String datReq;  //String for our data
int packetSize; //Size of the packet

//====================================================================
//============================= Definitions ==========================
//====================================================================

// Sensores
cyclopSensor cdom(cdom_id, cdom_read, cdom_x10, cdom_x100);
cyclopSensor phy(phy_id, phy_read, phy_x10, phy_x100);
cyclopSensor chl(chl_id, chl_read, chl_x10, chl_x100);

MS5837 presion;
//TSYS01 temp;

bool presion_fail = false;

// Utilidades
EthernetUDP udp;

// SDI-12
SDI12 mySDI12(SDI_DATA_PIN);

//====================================================================
//=========================== Setup ==================================
//====================================================================

void setup(){
  // Local configs
  byte mac[6] = { 0x90, 0xA2, 0xDA, 0x2A, 0xB8, 0xCE };
  unsigned int local_port = 55055;

  // IP Static config (Router Amarillo == DHCP)
  IPAddress ip(10, 0, 1, 10);
  IPAddress gw(10, 0, 1, 1);
  IPAddress my_dns(1, 1, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  unsigned int server_port = 45045;

  ///////// START
  
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));
  delay(300);

  // Configuracion cyclops
  cdom.setMaxPPB(cdom_maxPPB);
  phy.setMaxPPB(phy_maxPPB);
  chl.setMaxPPB(chl_maxPPB);

  Serial.print(F("CDOM Gain: x"));
  Serial.println((String) cdom.getGain());
  Serial.print(F("Phy Gain: x"));
  Serial.println((String) phy.getGain());
  Serial.print(F("CHL Gain: x"));
  Serial.println((String) chl.getGain());

  // Configuracion Ethernet
  Serial.println(F("Initialize Ethernet with DHCP:"));

  // Static w5100
  //Ethernet.begin(mac, ip, my_dns, gw, subnet); 
  
  // DHCP
  if(Ethernet.begin(mac) == 0){
    // 1 min aprox to fail
    Serial.println(F("Error on Ethernet. DHCP Server not found."));
    Serial.println(F("Setting static IP Adress."));
    Ethernet.begin(mac, ip, my_dns, gw, subnet); 
    //while(true){
    //  delay(1);
    //}
  }

  delay(1000);

  udp.begin(local_port);
  Serial.print(F("IP: "));
  Serial.println(Ethernet.localIP());
  Serial.print(F("Port: "));
  Serial.println(local_port);

  // Configuracion I2C sensors
  Wire.begin();
  delay(100);

  int retry = 0;
  
  // Go UP Sensors
  //temp.init();
  while (!presion.init()){
    if(retry > 10) {
      presion_fail = true;
      break;
    } else {
      retry++;
    }

    Serial.println(F("Presion sensor failed"));
    delay(2000);
  }
  
  if(presion_fail == false){
    presion.setModel(MS5837::MS5837_30BA);
    presion.setFluidDensity(1029); // kg/m^3 (1029 for seawater)
  }

  // SDI-12
  Serial.println(F("Opening SDI-12 bus..."));
  mySDI12.begin();

  // Power UP SDI-12 Sensors
  if(SDI_POWER_PIN > 0){
    Serial.println(F("Powering up SDI-12 sensors..."));
    pinMode(SDI_POWER_PIN, OUTPUT);
    digitalWrite(SDI_POWER_PIN, HIGH);
  }

  Serial.println(F("Ready: "));
  delay(500);
}

//====================================================================
//============================= Loop =================================
//====================================================================

void loop(){
  packetSize = udp.parsePacket();

  if (packetSize > 0){
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); 
    datReq = packetBuffer;

    Serial.print(F("Received: "));
    Serial.println(datReq);
    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
    
    // Mandamos lo recibido a la funcion
    // Si es alguno de nuestros sensores, mandamos la medida
    // Si no es, no hacemos nada
    read_sensor(datReq);
  }
}

//====================================================================
//====================== Functions as helpers! =======================
//====================================================================
String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String read_ce_sdi12(int addr){
    String sdiResponse = "";           // clear the response string
    mySDI12.sendCommand(String(addr) + "M!");
    delay(30);                     // wait a while for a response
    while (mySDI12.available()) {  // build response string
        char c = mySDI12.read();
        if ((c != '\n') && (c != '\r')) {
          sdiResponse += c;
          delay(5);
        }
    }
   
    mySDI12.clearBuffer();
    delay(50);                 // delay between taking reading and requesting data
    
    // next command to request data from last measurement
    mySDI12.sendCommand(String(addr) + "D0!");
    delay(30);                     // wait a while for a response
    while (mySDI12.available()) {  // build string from response
        char c = mySDI12.read();
        if ((c != '\n') && (c != '\r')) {
            sdiResponse += c;
            delay(5);
        }
    }
    
    if (sdiResponse.length() > 1){
        mySDI12.clearBuffer();
        // Return CE, second value on sdiResponse
        return getValue(sdiResponse, '+', 1);
    } else {
        mySDI12.clearBuffer();
        return "-1";
    }
}

void send_message(String msg){
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.print(msg);
  udp.endPacket();
  //memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
}

void read_sensor(String mSensor){
  // Formato de los mensajes:
  // <type>;<gain>;<measure>;<mv> (cyclops)
  // <type>;<pressure>;<temp>;<depth>;<altitude>
  // <type>;<measure> (i2c sensors)
  String message;
  
  if(mSensor == "cdom"){
    String cdom_measure = (String) cdom.measure();
    String cdom_mv = (String) cdom.getMV();
    String cdom_gain = (String) cdom.getGain();

    message = "cdom;" + cdom_gain + ";" + cdom_measure + ";" + cdom_mv;
    Serial.println("CDOM => x" + cdom_gain + " value: " + cdom_measure + " mV: " + cdom_mv);
    send_message(message);
  }

  if(mSensor == "phy"){
    String phy_measure = (String) phy.measure();
    String phy_mv = (String) phy.getMV();
    String phy_gain = (String) phy.getGain();

    message = "phy;" + phy_gain + ";" + phy_measure + ";" + phy_mv;
    Serial.println("PHY => x" + phy_gain + " value: " + phy_measure + " mV: " + phy_mv);
    send_message(message);
  }

  if(mSensor == "chl"){
    String chl_measure = (String) chl.measure();
    String chl_mv = (String) chl.getMV();
    String chl_gain = (String) chl.getGain();

    message = "chl;" + chl_gain + ";" + chl_measure + ";" + chl_mv;
    Serial.println("CHL => x" + chl_gain + " value: " + chl_measure + " mV: " + chl_mv);
    send_message(message);
  }

  if(mSensor == "ms5"){
    if(presion_fail == true){
      // Presion sensor doesnt boot. ERROR
      message = "ms5;error";
      Serial.println(F("MS5837 => Error, sensor doesnt boot"));

       send_message(message);
    } else {
      presion.read();
      String ms_pressure = (String) presion.pressure();
      String ms_temp = (String) presion.temperature();
      String ms_depth = (String) presion.depth();
      String ms_alt = (String) presion.altitude();

      message = "ms5;" + ms_pressure + ";" + ms_temp + ";" + ms_depth + ";" + ms_alt;
      Serial.println("MS5837 => " + ms_pressure + " mbar, " + ms_temp + " C, " +
                 ms_depth + " m, " + ms_alt + " m above sea");
      send_message(message);
    } 
  }

  //if(mSensor == "temp"){
  //  temp.read();
  //  ts_temp = (String) temp.temperature();

  //  message = "temp;" + ts_temp;
  //  Serial.println("TSYS01 => " + ts_temp + " C");
  //  send_message(message);
  //}

  if(mSensor.equals("ce")){
    String ce_measure =  read_ce_sdi12(CE_ADDRESS);

    message = "ce;" + String(CE_ADDRESS) + ";" + ce_measure;
    Serial.println("CE => ID: " + String(CE_ADDRESS) + " value: " + ce_measure);
    send_message(message);
  }

  if(mSensor == "ping"){
    message = "pong;v3";
    Serial.println(F("PING => PONG!"));
    send_message(message);
  }
}
