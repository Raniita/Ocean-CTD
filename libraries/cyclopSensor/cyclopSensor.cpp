/* 
* cyclopSensor.cpp
* Library for take measures with Turner Cyclops Sensors
*/
#include "Arduino.h"
#include "cyclopSensor.h"

/*
* Constructor
* 
*/
cyclopSensor::cyclopSensor(char mId, byte mPin, byte mx10, byte mx100)
{
	// Saving the parameters
	// Sensor declarations
	id = mId;
	pin = mPin;
	x10 = mx10;
	x100 = mx100;

	maxMV = 5000;		// mV maximos segun las especs
	switchDelay = 450;  // In seconds
	gain = 1;

	// Declarations of I/O
	pinMode(pin, INPUT);   // Pin de lectura del sensor
	pinMode(x10, OUTPUT);  // Selector de ganacia x10
	pinMode(x100, OUTPUT); // Selector de ganacia x100

	/*
	* Tabla de ganacias (Nunca tener los dos en LOW a la vez)
	* x1   -> x10 y x100 HIGH
	* x10  -> x10 LOW y x100 HIGH
	* x100 -> x10 HIGH y x100 LOW
	*/

	// Configuracion default para las lecturas
	// Default x10 ya que es el tipo más frecuente
	gain = 10;
	digitalWrite(x100, HIGH);
	digitalWrite(x10, LOW);
}

// BORRAR???
double cyclopSensor::map_double(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double cyclopSensor::calcPPB(uint8_t mGain, uint16_t mmV, int mmaxPPB)
{
	switch(mmaxPPB){
	// Equation of the line extracted of the manual of each sensor.
		case 1500 :
			// CDOM
			if(mGain == 10){
				ppb = mmV * 0.0312 - 6.1458;
			} else if(mGain == 100) {
				ppb = mmV * 0.0033 - 1.5556;
			} else {
				ppb = mmV * 0.3021 - 10.4733;
			}
			return ppb;
		
		case 750 : 
			// PHY
			if(mGain == 10){
				ppb = mmV * 0.0152 - 1.04061;
			} else if(mGain == 100) {
				ppb = mmV * 0.0016 - 0.72222;
			} else {
				ppb = mmV * 0.1510 - 5.18630;
			}
			return ppb;

		case 500 :
			// CHL
			if(mGain == 10){
				ppb = mmV * 0.0102 - 0.7614;
			} else if(mGain == 100) {
				ppb = mmV * 0.0010 - 0.2083;
			} else {
				ppb = mmV * 0.1007 - 3.5247;
			}
			return ppb;

		default:
			return 0;
	}
}

// BORRAR????
// Metodo extraido de la primera version del codigo
void cyclopSensor::autoGain()
{
	// Muestreamos para ajustar ganacia
	lastValue = map(analogRead(pin), 0, 1023, 0, maxMV);

	// Subida de x1 a x10
	if (gain == 1 && lastValue < 499)
	{
		switchGain(10);
		//return;
	}
	// Subida x10 a x100
	else if (gain == 10 && lastValue < 499)
	{
		switchGain(100);
		//return;
	}
	// Bajada de x100 a x10
	else if (gain == 100 && lastValue > maxMV - 1)
	{
		switchGain(10);
		//return;
	}
	// Bajada de x10 a x1
	else if (gain == 10 && lastValue > maxMV - 1)
	{
		switchGain(1);
		//return;
	}
}

// Mi implementacion de autoganancia
void cyclopSensor::adjustGain()
{
	// Muestro del ultimo valor
	lastValue = map(analogRead(pin), 0, 1023, 0, maxMV);

	// Autoadjust gain
	if (gain == 1)
	{
		// Gain at x1
		switchGain(1);

		if (lastValue < 290)
		{
			// Switch to x10
			switchGain(10);
			//return;
		}
	}

	if (gain == 10)
	{
		// Gain at x10
		switchGain(10);
		if (lastValue < 290)
		{
			// Switch to x100
			switchGain(100);
			//return;
		}
		else if (lastValue > maxMV - 500)
		{
			// Switch to x1
			switchGain(1);
			//return;
		}
	}

	if (gain == 100)
	{
		// Gain at x100
		switchGain(100);
		if (lastValue > maxMV - 500)
		{
			// Switch to x10
			switchGain(10);
			//return;
		}
	}
}

// BORRAR???
// Metodo para obtener el valor con autoGain()
double cyclopSensor::sample()
{
	autoGain();
	delay(50);

	read = analogRead(pin);
	// Guardamos mV
	mV = map(read, 0, 1023, 0, maxMV);
	// Devolvemos ppb
	return map_double(read, 0, 1023, 0, maxPPB / gain);
}

// Conjunto de metodos para mi implementacion de autoganancia
// DISCLAIMER: El metodo da mV coherentes, comprobado con DT
double cyclopSensor::measure()
{
	adjustGain();
	read = analogRead(pin);
	// Guardamos mV
	mV = map(read, 0, 1023, 0, maxMV);
	// Devolvemos ppb
	return calcPPB(gain, mV, maxPPB);
}

double cyclopSensor::measure(bool autogain)
{
	// Si autogain -> true ajustamos la ganancia y medimos
	// Si !autogain -> medimos con la ganacia anterior
	if (autogain)
	{
		adjustGain();
	}

	read = analogRead(pin);
	// Guardamos mV
	mV = map(read, 0, 1023, 0, maxMV);
	// Devolvemos ppb
	return calcPPB(gain, mV, maxPPB);
}

// BORRARR???
double cyclopSensor::convert2ppb(double measure)
{
	return map_double(measure, 0, 1023, 0, maxPPB / gain);;
}

uint8_t cyclopSensor::getGain()
{
	return gain;
}

void cyclopSensor::setGain(uint8_t mGain)
{
	switchGain(mGain);
}

double cyclopSensor::getMaxPPB()
{
	return maxPPB;
}

void cyclopSensor::setMaxPPB(double mPPB)
{
	maxPPB = mPPB;
}

double cyclopSensor::getMV()
{
	return mV;
}

void cyclopSensor::switchGain(uint8_t mGain)
{
	switch (mGain)
	{
	case 1:
		gain = 1;
		digitalWrite(x10, HIGH);
		digitalWrite(x100, HIGH);
		delay(switchDelay);
		break;
	case 10:
		gain = 10;
		digitalWrite(x100, HIGH);
		digitalWrite(x10, LOW);
		delay(switchDelay);
		break;
	case 100:
		gain = 100;
		digitalWrite(x10, HIGH);
		digitalWrite(x100, LOW);
		delay(switchDelay);
		break;
	}
}
