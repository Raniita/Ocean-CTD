/* 
* cyclopSensor.cpp
* Library for take measures with Turner Sensors
*
* 
*/

#include "Arduino.h"
#include "cyclopSensor.h"

/*
* Constructor
* 
*/
cyclopSensor::cyclopSensor(byte mId, uint32_t mSN, byte mPin, byte mx10, byte mx100, double mMaxPPB){
	// Saving the parameters
	id = mId;
	sn = mSN;
	pin = mPin;
	x10 = mx10;
	x100 = mx100;
	maxPPB = mMaxPPB;
	
	maxMV = 5000; 		   // mV maximos segun las especs
	switchDelay = 1500; 	// In seconds
	gain = 1; 
	
	// Declarations of I/O
	pinMode(pin, INPUT);   // Pin de lectura del sensor
	pinMode(x10, OUTPUT);  // Selector de ganacia x10
	pinMode(x100, OUTPUT); // Selector de ganacia x100
	
	/*
	* Tabla de ganacias
	* x1   -> x10 y x100 LOW
	* x10  -> x10 HIGH y x100 LOW
	* x100 -> x10 LOW y x100 HIGH
	*/
	
	// Configuracion default para las lecturas
	// Default x10 ya que es el tipo más frecuente
	digitalWrite(x10, HIGH);
	digitalWrite(x100, LOW);
	gain = 10;
}

double cyclopSensor::map_double(double x, double in_min, double in_max, double out_min, double out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void cyclopSensor::adjustGain(){
	// Muestro del ultimo valor
	lastValue = map(analogRead(pin),0,1023,0, maxMV);
	
	// Check value
	if(gain == 1 && lastValue < 499){
		// Cambiamos a 10
		digitalWrite(x10, LOW);
		digitalWrite(x100, HIGH);
		gain = 10;
		delay(switchDelay);
	} else if(gain == 10 && lastValue < maxMV - 1){
		// Cambiamos a 100
		digitalWrite(x10, HIGH);
		digitalWrite(x100, LOW);
		gain = 100;
		delay(switchDelay);
	} else if(gain == 100 && lastValue < maxMV - 1){
		// Cambiamos a 10
		digitalWrite(x10, LOW);
		digitalWrite(x100, HIGH);
		gain = 10;
		delay(switchDelay);
	} else if(gain == 10 && lastValue < maxMV - 1){
		// Cambiamos a 1;
		digitalWrite(x10, HIGH);
		digitalWrite(x100, HIGH);
		gain = 1;
		delay(switchDelay);
	}
	
}

double cyclopSensor::measure(){
	adjustGain();
	return value = map(analogRead(pin), 0, 1023, 0, maxMV);
}

double cyclopSensor::measure(bool ppb){
	adjustGain();
	value = map(analogRead(pin), 0, 1023, 0, maxMV);
	if(ppb == true){
		// Convertimos a ppb
		value = map_double(analogRead(pin), 0, 1023, 0, maxPPB/(int)gain);
		return value; 	// Devolvemos PPB
	} else {
		return value;	// Devolvemos mV
	}
}

double cyclopSensor::convert2ppb(double measure){
	// Hay que preguntar
	return measure;
}
