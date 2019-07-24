#include <LowPower.h>

// Definimos el sensor
// En este caso utilizamos el sensor de phyto
#define sensor_name_1 "PHY"
#define sensor_id_1 1
#define sensor_sn_1 21180134
#define sensor_readPin_1 A1
#define sensor_x10_1 8
#define sensor_x100_1 9
#define sensor_maxPPB_1 750

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Init.");

  pinMode(A1, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);

  delay(100);
}

void loop() {
  Serial.println(map(analogRead(A1),0,1023,0, 5000));

  delay(200);
  // Sleep arduino
  //LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); 
}
