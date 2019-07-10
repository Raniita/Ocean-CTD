
#ifndef __easy2Time_H__
#define __easy2Time_H__

#include <Arduino.h>
#include <RTClib.h>


class easy2Time
{
private:
	uint8_t easy2Time_I2C_ADDRESS;

public:

	easy2Time(uint8_t _easy2Time_I2C_ADDRESS);

	void begin();
	void setTime(), getTime();
	uint8_t second;
	uint8_t minute;
	uint8_t hour; 
	uint8_t dayOfWeek;// day of week, 1 = Monday
	uint8_t dayOfMonth;
	uint8_t month;
	uint16_t year;
	RTC_PCF8523 rtc;
	
	String nowIs();
	String todayDir();
	String nowFile();
	void setTimeAPP(uint8_t _day, uint8_t _month, uint8_t _year, uint8_t _hour, uint8_t _minute, uint8_t _second);
};



#endif // _RTCLIB_H_