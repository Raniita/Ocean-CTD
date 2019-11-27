#ifndef cyclopSensor_h
#define cyclopSensor_h
#include "Arduino.h"

class cyclopSensor {
	public:
		// Constructor
		cyclopSensor(byte mId, byte mPin, byte mx10, byte x100);
		//vars
		
		//func
		double sample(), measure(), measure(bool autogain), measure(bool autogain, bool ppb), convert2ppb(double measure);
		uint8_t getGain();
		double getMaxPPB(), getMV();
		void setMaxPPB(double mPPB), setGain(uint8_t mGain);

	private:
		//vars
		uint8_t id, pin, x10, x100, gain;
		double maxPPB, maxMV, ppb, read;
		//char sn[];
		uint16_t switchDelay, lastValue, value, mV;

		//func
		double map_double(double x, double in_min, double in_max, double out_min, double out_max);
		void autoGain(), adjustGain(), switchGain(uint8_t mGain);
};

#endif
