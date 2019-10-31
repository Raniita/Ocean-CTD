//====================================================================
//============================= Librarys =============================
//====================================================================
#include <avr/pgmspace.h>
#include <TSYS01.h>
#include <MS5837.h>
#include <cyclopSensor.h>
//#include <LowPower.h>

#include<Wire.h>
#include <SPI.h>
// Ethernet2 library for w5100
#include <Ethernet.h>

// Ethernet3 library for w5500
//#include <Dhcp.h>
//#include <Dns.h>
//#include <Ethernet3.h>
//#include <EthernetClient.h>
//#include <EthernetServer.h>
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

// IP Configs
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
IPAddress ip_server(192, 168, 1, 5);   // Server IP

//====================================================================
//============================= Definitions ==========================
//====================================================================

// Sensores
cyclopSensor cdom(cdom_id, cdom_read, cdom_x10, cdom_x100);
cyclopSensor phy(phy_id, phy_read, phy_x10, phy_x100);
cyclopSensor chl(chl_id, chl_read, chl_x10, chl_x100);
MS5837 presion;
TSYS01 temp;

// Utilidades
EthernetClient client;

void setup() {
  // Configuracion puerto serie
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));
  delay(50);

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
  while(!presion.init()) {
    Serial.println("Presion sensor failed");
    delay(2000);
  }
  presion.setModel(MS5837::MS5837_30BA);
  presion.setFluidDensity(1029); // kg/m^3 (freshwater, 1029 for seawater)

  // Configuracion Ethernet
  Ethernet.begin(mac, ip);

  Serial.println(" ");
  delay(2000);
}

void loop() {
  

}
