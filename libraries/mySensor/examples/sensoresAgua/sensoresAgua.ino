#include "mySensor.h"
// Prueba de la libreria jiji

// SENSOR 1:
#define sensorName "CDOM"
#define sn "21180135"
#define readPin A0
#define x10pin 6
#define x100pin 7
#define maxPPB 3000


mySensor sensor1(sensorName, sn, readPin, x10pin, x100pin, maxPPB); // puerto 1 de prueba XD
//mySensor sensor2(2_readPin, 2_x10pin, 2_x100pin, 2_maxPPB); // puerto 2 de prueba XD
mySensor hola("ola k ase", "65dcsecw5", A1, 8, 9, 750);
void setup()
{
Serial.begin(9600);
  Serial.println(sensor1.info());
  Serial.println(hola.info());
  hola.setSwitchingDelay(500);
   

}

void loop()
{ 
  Serial.println("Valor medido: " + sensor1.read());
  Serial.println("Valor medido y convertido " + (String)sensor1.readDouble());
  Serial.println("Valor del sensor imaginario: " + hola.read());

}



