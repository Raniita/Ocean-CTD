#ifndef cyclopSensor_h
#define cyclopSensor_h
#include "Arduino.h"

class cyclopSensor {
	public:
		// Constructor
		cyclopSensor(byte mId, uint32_t mSN, byte mPin, byte mx10, byte x100, double mMaxPPB);
		//vars
		
		//func
		double measure(), measure(bool ppb), convert2ppb(double measure);
		
	private:
		//vars
		byte id, pin, x10, x100;
		int gain;
		double maxPPB, maxMV;
		uint32_t sn;
		int switchDelay, lastValue, value;
		//func
		double map_double(double x, double in_min, double in_max, double out_min, double out_max);
		void adjustGain();
};
#endif
