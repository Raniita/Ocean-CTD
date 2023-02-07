//====================================================================
//============================= Librarys =============================
//====================================================================
#include <cyclopSensor.h>
#include <avr/pgmspace.h>
#include <SDI12.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

// Serial configuration
#define BAUDRATE 115200

// Cyclops Sensors
// Plateado
#define cdom_type "cdom"    // UPDATE DIODO: x10 420mV {ppb: 13.104} [std: 373mV (x10) {ppb: 11.6376}]
#define cdom_id "1"
#define cdom_read A1
#define cdom_x10 5          // Naranja. He cambiado 6 por 5
#define cdom_x100 6         // Amarillo. He cambiado 5 por 6
#define cdom_maxPPB 1500

// Negro
#define phy_type "phy"      // UPDATE DIODO: x100 1783mV {ppb: 2.8528} [std: 1430mV (x100) {ppb: 2.288}] 
#define phy_id "2"
#define phy_read A0
#define phy_x10 7           // Naranja. He cambiado 8 por 7
#define phy_x100 8          // Amarillo. He cambiado 7 por 8
#define phy_maxPPB 750

// Amarillo
#define chl_type "chl"      // UPDATE DIODO: x100 654mV {ppb: 0.654} [std: 630mV (x100) {ppb: 0.63}]
#define chl_id "3"
#define chl_read A2
#define chl_x10 2           // Naranja. He cambiado el 3 por 2
#define chl_x100 3          // Amarillo. He cambiado el 3 por 2
#define chl_maxPPB 500

// SDI-12. Sensor Conductividad
#define SDI_DATA_PIN 9
#define SDI_POWER_PIN 4
#define CE_ADDRESS 0
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

// SDI-12
SDI12 mySDI12(SDI_DATA_PIN);
String ce_measure;

//====================================================================
//=========================== Setup ==================================
//====================================================================

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));
  delay(300);

  // Configuracion cyclops
  cdom.setMaxPPB(cdom_maxPPB);
  phy.setMaxPPB(phy_maxPPB);
  chl.setMaxPPB(chl_maxPPB);

  Serial.println("CDOM Gain: x" + (String) cdom.getGain());
  Serial.println("Phy Gain: x" + (String) phy.getGain());
  Serial.println("CHL Gain: x" + (String) chl.getGain());

  // SDI-12
  Serial.println("Opening SDI-12 bus...");
  mySDI12.begin();

  // Power UP SDI-12 Sensors
  if(SDI_POWER_PIN > 0){
    Serial.println("Powering up SDI-12 sensors...");
    pinMode(SDI_POWER_PIN, OUTPUT);
    digitalWrite(SDI_POWER_PIN, HIGH);
    delay(200);
  }

  Serial.println(F("Ready: [Serial Version]"));
  delay(500);
}

//====================================================================
//============================= Loop =================================
//====================================================================

void loop() {
  if (Serial.available() > 0) {
    String static datReq;
    
    // read the incoming byte:
    datReq = Serial.readString();
    datReq.trim();

    // say what you got:
    Serial.println("Received: " + datReq);

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
    
    //if (sdiResponse.length() > 1){
    //    Serial.println(sdiResponse); //write the response to the screen
    //}
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
        //Serial.println(sdiResponse); //write the response to the screen
        mySDI12.clearBuffer();
        // Return CE, second value on sdiResponse
        return getValue(sdiResponse, '+', 1);
    } else {
        mySDI12.clearBuffer();
        return "-1";
    }
}

void send_message(String msg){
  // Version of send_message to work with Serial
  Serial.println("Send: " + msg);
}

void read_sensor(String mSensor){
  // Formato de los mensajes:
  // <type>;<gain>;<measure>;<mv> (cyclops)
  // <type>;<pressure>;<temp>;<depth>;<altitude>
  // <type>;<measure> (i2c sensors)

   String static message;
  
  if(mSensor.equals("cdom")){
    cdom_measure = (String) cdom.measure();
    cdom_mv = (String) cdom.getMV();
    cdom_gain = (String) cdom.getGain();

    message = "cdom;" + cdom_gain + ";" + cdom_measure + ";" + cdom_mv;
    Serial.println("CDOM => x" + cdom_gain + " value: " + cdom_measure + " mV: " + cdom_mv);
    send_message(message);
  }

  if(mSensor.equals("phy")){
    phy_measure = (String) phy.measure();
    phy_mv = (String) phy.getMV();
    phy_gain = (String) phy.getGain();

    message = "phy;" + phy_gain + ";" + phy_measure + ";" + phy_mv;
    Serial.println("PHY => x" + phy_gain + " value: " + phy_measure + " mV: " + phy_mv);
    send_message(message);
  }

  if(mSensor.equals("chl")){
    chl_measure = (String) chl.measure();
    chl_mv = (String) chl.getMV();
    chl_gain = (String) chl.getGain();

    message = "chl;" + chl_gain + ";" + chl_measure + ";" + chl_mv;
    Serial.println("CHL => x" + chl_gain + " value: " + chl_measure + " mV: " + chl_mv);
    send_message(message);
  }

  if(mSensor.equals("ce")){
    ce_measure =  read_ce_sdi12(CE_ADDRESS);

    message = "ce;" + String(CE_ADDRESS) + ";" + ce_measure;
    Serial.println("CE => ID: " + String(CE_ADDRESS) + " value: " + ce_measure);
    send_message(message);
  }

  if(mSensor.equals("ping")){
    message = "pong";
    Serial.println(F("PING => PONG!"));
    send_message(message);
  }
}
