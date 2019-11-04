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
// Ethernet2 library for w5100
#include <Ethernet.h>
#include <EthernetUdp.h>

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
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
//IPAddress dns(8, 8, 8, 8);
IPAddress gw(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip_server(192, 168, 1, 5); // Server IP
unsigned int local_port = 55055;
unsigned int server_port = 45045;
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
TSYS01 temp;

// Utilidades
//EthernetClient client;
EthernetUDP udp;

void setup()
{
  // Configuracion puerto serie
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));
  delay(50);

  // Configuracion cyclops
  cdom.setMaxPPB(cdom_maxPPB);
  phy.setMaxPPB(phy_maxPPB);
  chl.setMaxPPB(chl_maxPPB);

  Serial.println("CDOM Gain: x" + (String)cdom.getGain());
  Serial.println("Phy Gain: x" + (String)phy.getGain());
  Serial.println("CHL Gain: x" + (String)chl.getGain());

  // Configuracion I2C sensors
  Wire.begin();
  temp.init();
  while (!presion.init())
  {
    Serial.println("Presion sensor failed");
    delay(2000);
  }
  presion.setModel(MS5837::MS5837_30BA);
  presion.setFluidDensity(1029); // kg/m^3 (freshwater, 1029 for seawater)

  // Configuracion Ethernet
  Ethernet.begin(mac, ip, INADDR_NONE, gw, subnet);

  // Check for Ethernet hardware present
  /*if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  */

  udp.begin(local_port);
  delay(1500);

  Serial.println(Ethernet.localIP());

  Serial.println(" ");
  delay(2000);
}

void loop()
{
  // Lo primero que hacemos es comprobar si nuevo paquete
  packetSize = udp.parsePacket();

  if (packetSize > 0)
  {
    // Tenemos un nuevo mensaje
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); // Read the request
    String datReq(packetBuffer);                    // Convert the buffer to datReq string

    // Aqui evaluamos en funcion de datReq
    Serial.println(datReq);
  }
  else
  {
    send_data();
    delay(2000);
  }
}

// Polling sensors + packet craft
void send_data()
{
  String cdom_measure, cdom_mv, cdom_gain;
  String phy_measure, phy_mv, phy_gain;
  String chl_measure, chl_mv, chl_gain;
  String ms_pressure, ms_temp, ms_depth, ms_alt;
  String ts_temp;
  String message;

  // Mandamos sensor a sensor los datos
  // Formato de los mensajes:
  // <type>;<gain>;<measure>;<mv> (cyclops)
  // <type>;<pressure>;<temp>;<depth>;<altitude>
  // <type>;<measure> (i2c sensors)

  // CDOM Sensor
  cdom_measure = (String)cdom.sample();
  cdom_mv = (String)cdom.measure();
  cdom_gain = (String)cdom.getGain();

  message = "cdom;" + cdom_gain + ";" + cdom_measure + ";" + cdom_mv;

  Serial.println("CDOM => x" + cdom_gain + " value: " + cdom_measure + " mV: " + cdom_mv);

  send_message(message);

  // Phyto Sensor
  phy_measure = (String)phy.sample();
  phy_mv = (String)phy.measure();
  phy_gain = (String)phy.getGain();

  message = "phy;" + phy_gain + ";" + phy_measure + ";" + phy_mv;

  Serial.println("Phy => x" + phy_gain + " value: " + phy_measure + " mV: " + phy_mv);

  send_message(message);

  // CHL Sensor
  chl_measure = (String)chl.sample();
  chl_mv = (String)chl.measure();
  chl_gain = (String)chl.getGain();

  message = "chl;" + chl_gain + ";" + chl_measure + ";" + chl_mv;

  Serial.println("CHL => x" + chl_gain + " value: " + chl_measure + " mV: " + chl_mv);

  send_message(message);

  // MS5837 Sensor
  presion.read();
  ms_pressure = (String)presion.pressure();
  ms_temp = (String)presion.temperature();
  ms_depth = (String)presion.depth();
  ms_alt = (String)presion.altitude();

  message = "ms5;" + ms_pressure + ";" + ms_temp + ";" + ms_depth + ";" + ms_alt;

  Serial.println("MS5837 => " + ms_pressure + " mbar, " + ms_temp + " C, " +
                 ms_depth + " m, " + ms_alt + " m above sea");

  send_message(message);

  // TSYS01 Sensor
  temp.read();
  ts_temp = (String)temp.temperature();

  message = "temp;" + ts_temp;

  Serial.println("TSYS01 => " + ts_temp + " C");

  send_message(message);
}

// Packet send
void send_message(String msg)
{
  udp.beginPacket(ip_server, server_port);
  udp.print(msg);
  udp.endPacket();
}
