#include "Arduino.h"
#include "mySensor.h"

mySensor::mySensor(byte _id,  uint32_t _sn, byte _pin, byte _x10pin, byte _x100pin,  double _maxPPB)
{
	
	id = _id;
  pin = _pin;
  maxPPB = _maxPPB;
  x10pin = _x10pin;
  x100pin = _x100pin;
  sn = _sn;
  switchingDelay = 1500;
  scale = 1;

  pinMode(_pin, INPUT); // pin de lectura
  pinMode(_x10pin, OUTPUT); // para establecer ganancia en el sensor.
  pinMode(_x100pin, OUTPUT);
  digitalWrite(_x10pin, HIGH); // elijo el x1
  digitalWrite(_x100pin, HIGH);
}


void mySensor::switchToScale(byte _scale){
switch(_scale){
  case 1: scale = 1;
      digitalWrite(x10pin, HIGH);
      digitalWrite(x100pin, HIGH);
      delay(switchingDelay);
  break;
  case 10: scale = 10;
      digitalWrite(x10pin, LOW);
    digitalWrite(x100pin, HIGH);
    delay(switchingDelay);
  break;
  case 100: scale = 100;
      digitalWrite(x10pin, HIGH);
    digitalWrite(x100pin, LOW);
    delay(switchingDelay);
  // aqui meter la secuencia electrónica de cambio.
  break;
}
}
 void mySensor::autoGain(){
   lastValue = map(analogRead(pin),0,1023,0,5000); // muestreo valor
// evaluo si hay que hacer un cambio de escala.
if(scale == 1 && lastValue < 499){ //SUBIDA
switchToScale(10);
}
else if(scale == 10 && lastValue < 499){ 
switchToScale(100);
}
else if(scale == 100 && lastValue > 4999){                 
 switchToScale(10);
}
else if(scale == 10 && lastValue > 4999){
switchToScale(1);
}

  
 }

double mySensor::map_double(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


double mySensor::readDouble()
{
  autoGain();
 return map_double(analogRead(pin), 0, 1023, 0, maxPPB/(int)scale);
 
}

double mySensor::readgain()
{
 
 return scale;
 
}

double mySensor::readgain()
{
 
 return scale;
 
}
