//====================================================================
//============================= Librarys =============================
//====================================================================
#include <avr/pgmspace.h>
#include <TSYS01.h>
#include <MS5837.h>
#include <cyclopSensor.h>
//#include <LowPower.h>

#include <Wire.h>
#include <SPI.h>

// Ethernet Librarys (w5100 == Ethernet, w5500 == Ethernet, Ethernet3) 
#include <Ethernet.h>
#include <EthernetUdp.h>
//#include <Ethernet3.h>
//#include <EthernetUdp3.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

// Serial configuration
#define BAUDRATE 115200

// Cyclops Sensors
#define cdom_type "cdom"
#define cdom_id "1"
#define cdom_read A0
#define cdom_x10 9
#define cdom_x100 8
#define cdom_maxPPB 1500

#define phy_type "phy"
#define phy_id "2"
#define phy_read A1
#define phy_x10 7
#define phy_x100 6
#define phy_maxPPB 750

#define chl_type "chl"
#define chl_id "3"
#define chl_read A2
#define chl_x10 5
#define chl_x100 3
#define chl_maxPPB 500

// Ethernet shield tiene la SD en el 4
#define sd_pin 4

// Local configs
#define hostname "upct_ctd"
byte mac[6] = { 0x90, 0xA2, 0xDA, 0x2A, 0xB8, 0xCE };
unsigned int local_port = 55055;

// IP Static config
IPAddress ip(192, 168, 1, 177);
IPAddress gw(192, 168, 1, 1);
IPAddress my_dns(1, 1, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// IP + Port Server
IPAddress ip_server(192, 168, 1, 5);
unsigned int server_port = 45045;

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

String cdom_measure, cdom_mv, cdom_gain;
String phy_measure, phy_mv, phy_gain;
String chl_measure, chl_mv, chl_gain;
String ms_pressure, ms_temp, ms_depth, ms_alt;
String ts_temp;
String message;

MS5837 presion;
TSYS01 temp;

// Utilidades
EthernetUDP udp;

void setup(){
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));
  delay(300);

  // Disable SD on Ethernet Shield
  pinMode(sd_pin, OUTPUT);
  digitalWrite(sd_pin, HIGH);

  // Configuracion cyclops
  cdom.setMaxPPB(cdom_maxPPB);
  phy.setMaxPPB(phy_maxPPB);
  chl.setMaxPPB(chl_maxPPB);

  Serial.println("CDOM Gain: x" + (String) cdom.getGain());
  Serial.println("Phy Gain: x" + (String) phy.getGain());
  Serial.println("CHL Gain: x" + (String) chl.getGain());

  // Configuracion I2C sensors
  Wire.begin();
  temp.init();
  while (!presion.init()){
    Serial.println(F("Presion sensor failed"));
    delay(2000);
  }
  presion.setModel(MS5837::MS5837_30BA);
  presion.setFluidDensity(1029); // kg/m^3 (freshwater, 1029 for seawater)

  // Configuracion Ethernet
  Serial.println(F("Initialize Ethernet with DHCP:")); 
  // Static w5100
  //Ethernet.begin(mac, ip, my_dns, gw, subnet); 
  
  // Static w5500 (Ethernet3)
  //Ethernet.begin(mac, ip, subnet, gw);
  
  if(Ethernet.begin(mac) == 0){
    Serial.println(F("Error on Ethernet. Not booting."));
    while(true){
      delay(1);
    }
  }
  
  delay(1500);

  udp.begin(local_port);
  Serial.println("IP: " + (String) Ethernet.localIP() + " port: " + local_port);
  Serial.println("IP Server: " + (String) ip_server + " port: " + server_port);

  Serial.println("");
  delay(2000);
}

void loop(){
  packetSize = udp.parsePacket();

  if (packetSize > 0){
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); 
    String datReq(packetBuffer);

    // Mandamos lo recibido a la funcion
    // Si es alguno de nuestros sensores, mandamos la medida
    // Si no es, no hacemos nada
    read_sensor(datReq);
  } 
}

void read_sensor(String mSensor){
  // Formato de los mensajes:
  // <type>;<gain>;<measure>;<mv> (cyclops)
  // <type>;<pressure>;<temp>;<depth>;<altitude>
  // <type>;<measure> (i2c sensors)
  
  if(mSensor == "cdom"){
    cdom_measure = (String) cdom.sample();
    cdom_mv = (String) cdom.measure();
    cdom_gain = (String) cdom.getGain();

    message = "cdom;" + cdom_gain + ";" + cdom_measure + ";" + cdom_mv;
    Serial.println("CDOM => x" + cdom_gain + " value: " + cdom_measure + " mV: " + cdom_mv);
    send_message(message);
  }

  if(mSensor == "phy"){
    phy_measure = (String) phy.sample();
    phy_mv = (String) phy.measure();
    phy_gain = (String) phy.getGain();

    message = "phy;" + phy_gain + ";" + phy_measure + ";" + phy_mv;
    Serial.println("PHY => x" + phy_gain + " value: " + phy_measure + " mV: " + phy_mv);
    send_message(message);
  }

  if(mSensor == "chl"){
    chl_measure = (String) chl.sample();
    chl_mv = (String) chl.measure();
    chl_gain = (String) chl.getGain();

    message = "chl;" + chl_gain + ";" + chl_measure + ";" + chl_mv;
    Serial.println("CHL => x" + chl_gain + " value: " + chl_measure + " mV: " + chl_mv);
    send_message(message);
  }

  if(mSensor == "ms5"){
    presion.read();
    ms_pressure = (String) presion.pressure();
    ms_temp = (String) presion.temperature();
    ms_depth = (String) presion.depth();
    ms_alt = (String) presion.altitude();

    message = "ms5;" + ms_pressure + ";" + ms_temp + ";" + ms_depth + ";" + ms_alt;
    Serial.println("MS5837 => " + ms_pressure + " mbar, " + ms_temp + " C, " +
                 ms_depth + " m, " + ms_alt + " m above sea");
    send_message(message);
  }

  if(mSensor == "temp"){
    temp.read();
    ts_temp = (String) temp.temperature();

    message = "temp;" + ts_temp;
    Serial.println("TSYS01 => " + ts_temp + " C");
    send_message(message);
  }
}

void send_message(String msg){
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.print(msg);
  udp.endPacket();

  delay(200);
}
