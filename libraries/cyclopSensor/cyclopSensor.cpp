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
cyclopSensor::cyclopSensor(byte mId, byte mPin, byte mx10, byte mx100)
{
	// Saving the parameters
	// Sensor declarations
	id = mId;
	pin = mPin;
	x10 = mx10;
	x100 = mx100;

	//mySensor.sn = mSN;
	//maxPPB = 1000;
	maxMV = 5000;		// mV maximos segun las especs
	switchDelay = 1500; // In seconds
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

double cyclopSensor::map_double(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Metodo extraido de la primera version del codigo
void cyclopSensor::autoGain()
{
	// Muestreamos para ajustar ganacia
	lastValue = map(analogRead(pin), 0, 1023, 0, maxMV);

	// Subida de x1 a x10
	if (gain == 1 && lastValue < 499)
	{
		switchGain(10);
	}
	// Subida x10 a x100
	else if (gain == 10 && lastValue < 499)
	{
		switchGain(100);
	}
	// Bajada de x100 a x10
	else if (gain == 100 && lastValue > maxMV - 1)
	{
		switchGain(10);
	}
	// Bajada de x10 a x1
	else if (gain == 10 && lastValue > maxMV - 1)
	{
		switchGain(1);
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

		if (lastValue < 300)
		{
			// Switch to x10
			switchGain(10);
		}
	}

	if (gain == 10)
	{
		// Gain at x10
		switchGain(10);
		if (lastValue < 300)
		{
			// Switch to x100
			switchGain(100);
		}
		else if (lastValue > maxMV - 500)
		{
			// Switch to x1
			switchGain(1);
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
		}
	}
}

// Metodo para obtener el valor con autoGain()
double cyclopSensor::sample()
{
	autoGain();
	delay(200);

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
	delay(200);

	read = analogRead(pin);
	// Guardamos mV
	mV = map(read, 0, 1023, 0, maxMV);
	// Devolvemos ppb
	return map_double(read, 0, 1023, 0, maxPPB / gain);
}

double cyclopSensor::measure(bool autogain)
{
	// Si autogain -> true ajustamos la ganancia y medimos
	// Si !autogain -> medimos con la ganacia anterior
	if (autogain)
	{
		adjustGain();
		delay(200);
	}

	read = analogRead(pin);
	// Guardamos mV
	mV = map(read, 0, 1023, 0, maxMV);
	// Devolvemos ppb
	return map_double(read, 0, 1023, 0, maxMV);
}

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