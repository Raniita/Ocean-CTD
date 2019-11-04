//====================================================================
//============================= Librarys =============================
//====================================================================
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp3.h>

//====================================================================
//============================= Librarys =============================
//====================================================================

#define BAUDRATE 115200

// IP Configs
// Cliente
byte mac[6] = { 0x90, 0xA2, 0xDA, 0x2A, 0xB8, 0xCE };
IPAddress ip(10, 0, 0, 240);
IPAddress gw(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
unsigned int local_port = 55055;

// Server
IPAddress ip_server(10, 0, 0, 126);
unsigned int server_port = 45045;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
String datReq;  // String for our data
int packetSize; // Size of the packet

//====================================================================
//============================= Definitions ==========================
//====================================================================

EthernetUDP udp;

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println(F("Initializated serial connection"));

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  Ethernet.macAddress(mac);
  Ethernet.begin(mac, ip, subnet, gw);
  delay(3500);
  
  //udp.begin(local_port);

  Serial.println("");
  delay(2000);
}

void loop() {
  // Funciones test ethernet3
  Serial.println(Ethernet.macAddressReport()); 
  Serial.println(Ethernet.phyState());
  Serial.println(Ethernet.link());
  Serial.println(Ethernet.linkReport());
  Serial.println(Ethernet.localIP());
  
  //send_message("cdom;10;34;3000");
  //delay(1000);
}

void send_message(String msg){
  Serial.println("Printing: " + msg);
  udp.beginPacket(ip_server, server_port);
  udp.print(msg);
  udp.endPacket();
}
