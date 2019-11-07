//====================================================================
//============================= Librarys =============================
//====================================================================
#include <avr/pgmspace.h>
#include <SPI.h>

// Ethernet Librarys (w5100 == Ethernet, w5500 == Ethernet, Ethernet3) 
#include <Ethernet.h>
#include <EthernetUdp.h>

//====================================================================
//============================= Librarys =============================
//====================================================================

#define BAUDRATE 115200

// Ethernet shield tiene la SD en el 4
#define sd_pin 4

// IP Configs
byte mac[6] = { 0x90, 0xA2, 0xDA, 0x2A, 0xB8, 0xCE };
unsigned int local_port = 55055;

// Static Config
IPAddress ip(192, 168, 0, 210);
IPAddress gw(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

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

  pinMode(sd_pin, OUTPUT);
  digitalWrite(sd_pin, HIGH);

  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  udp.begin(local_port);

  Serial.println("");
  delay(2000);
}

void loop() {
  packetSize = udp.parsePacket();

  if(packetSize > 0){
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    String datReq(packetBuffer);

    if(datReq == "hola"){
      send_message("quetal");
    }

    if(datReq == "adios"){
      send_message("adios");
    }
  }
  
  //send_message("cdom;10;34;3000");
  //delay(1000);
}

void send_message(String msg){
  Serial.println("Sending: " + msg);
  udp.beginPacket(ip_server, server_port);
  udp.print(msg);
  udp.endPacket();
}
