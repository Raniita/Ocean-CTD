
#ifndef __easyTime_H__
#define __easyTime_H__

#include <Arduino.h>



#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6
#define SUN 7



class easyTime
{
private:
	uint8_t decToBcd(uint8_t val);
	uint8_t bcdToDec(uint8_t val);
	uint8_t easyTime_I2C_ADDRESS;

public:

	easyTime(uint8_t _easyTime_I2C_ADDRESS);

	void begin();

	void setTime(void);
	void getTime(void);
	void fillByHMS(uint8_t _hour, uint8_t _minute, uint8_t _second);
	void fillByYMD(uint16_t _year, uint8_t _month, uint8_t _day);
	void fillDayOfWeek(uint8_t _dow);
	uint8_t second;
	uint8_t minute;
	uint8_t hour; 
	uint8_t dayOfWeek;// day of week, 1 = Monday
	uint8_t dayOfMonth;
	uint8_t month;
	uint16_t year;
	
	String nowIs();
	String todayDir();
	String nowFile();
	void setTimeAPP(uint8_t _day, uint8_t _month, uint8_t _year, uint8_t _hour, uint8_t _minute, uint8_t _second);
};

#endif
