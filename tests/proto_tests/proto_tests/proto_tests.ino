// Prototype tests before PCB.
// Ethernet ECHO
//====================================================================
//============================= Librarys =============================
//====================================================================
#include <avr/pgmspace.h>
//#include <Wire.h>
#include <SPI.h>

// Ethernet Librarys (w5100 == Ethernet, w5500 == Ethernet, Ethernet3) 
//#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <Ethernet3.h>
#include <EthernetUdp3.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

// Serial configuration
#define BAUDRATE 115200

// Local configs
//v1
byte mac[6] = { 0x90, 0xA2, 0xDA, 0x2A, 0xB8, 0xCE };
//v2
//byte mac[6] = { 0x90, 0xA2, 0xDA, 0x2A, 0xB8, 0xDE };

unsigned int local_port = 55055;

// IP Static config (Router Amarillo == DHCP)
IPAddress ip(10, 0, 1, 10);
IPAddress gw(10, 0, 1, 1);
IPAddress my_dns(1, 1, 1, 1);
IPAddress subnet(255, 255, 255, 0);

unsigned int server_port = 45045;

// UDP configs
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
String datReq;  //String for our data
int packetSize; //Size of the packet

//====================================================================
//============================= Definitions ==========================
//====================================================================

// Utilidades
EthernetUDP udp;

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));
  delay(300);

  // Configuracion Ethernet
  Serial.println(F("Initialize Ethernet with DHCP:")); 
  
  // Static w5100
//  Ethernet.begin(mac, ip, my_dns, gw, subnet); 
  // Static w5500 (Ethernet3)
  Ethernet.begin(mac, ip, subnet, gw);
  
  // DHCP
  //if(Ethernet.begin(mac) == 0){
  //  Serial.println(F("Error on Ethernet. Not booting."));
  //  while(true){
  //    delay(1);
  //  }
  //}

  delay(1000);

  udp.begin(local_port);
  Serial.print(F("IP: "));
  Serial.println(Ethernet.localIP());
  Serial.print(F("Port: "));
  Serial.println(local_port);

  delay(1000);
  Serial.println("Ready: ");
}

void loop() {
  packetSize = udp.parsePacket();

  if (packetSize > 0){
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); 
    datReq = packetBuffer;

    Serial.println("Received: " + datReq);
    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
    
    // Mandamos lo recibido a la funcion
    // Si es alguno de nuestros sensores, mandamos la medida
    // Si no es, no hacemos nada
    read_sensor(datReq);
  }
}

void read_sensor(String mSensor){
  send_message(mSensor + ";echo");
}

void send_message(String msg){
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.print(msg);
  udp.endPacket();
  //memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
}
