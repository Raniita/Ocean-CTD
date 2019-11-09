#ifndef mySensor_h
#define mySensor_h
#include "Arduino.h"

class mySensor
{
public:
mySensor(byte _id, uint32_t _sn, byte _pin, byte _x10pin, byte _x100pin,  double _maxPPB);
void setSwitchingDelay(int delayValue);
double readDouble();
double readgain();
byte pin, x10pin, x100pin, scale, id;
double maxPPB;
uint32_t sn;


private:
void switchToScale(byte _scale), autoGain();
double map_double(double x, double in_min, double in_max, double out_min, double out_max);
int switchingDelay, lastValue;
};
#endif
