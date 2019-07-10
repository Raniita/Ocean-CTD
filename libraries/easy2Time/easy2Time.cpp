#include <Wire.h>
#include "easy2Time.h"
#include <RTClib.h>

easy2Time::easy2Time(uint8_t _easy2Time_I2C_ADDRESS){
	easy2Time_I2C_ADDRESS = _easy2Time_I2C_ADDRESS;
	RTC_PCF8523 rtc;
 
 }


void easy2Time::begin()
{
	#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();
}

/****************************************************************/
/*Function: Read time and date from RTC	*/


String easy2Time::nowIs(){
	getTime();
	String dayOfMonthOut = "";
	String monthOut= "";
	String yearOut= "";
	String hourOut= "";
	String minuteOut= "";
	String secondOut= "";
	
	
	if(dayOfMonth < 10) dayOfMonthOut = "0" + (String)dayOfMonth;
	else dayOfMonthOut = (String)dayOfMonth;

	if(month < 10) monthOut = "0" + (String)month;
	else monthOut = (String)month;
	
	if(year < 10) yearOut = "0" + (String)year;
	else yearOut = (String)year;
	
	if(hour < 10) hourOut = "0" + (String)hour;
	else hourOut = (String)hour;
	
	if(minute < 10) minuteOut = "0" + (String)minute;
	else minuteOut = (String)minute;
	
	if(second < 10) secondOut = "0" + (String)second;
	else secondOut = (String)second;
	
	return dayOfMonthOut + "/" + monthOut + "/" + yearOut + "\t" + hourOut +":" + minuteOut +":" + secondOut +" ";
}

String easy2Time::todayDir(){
	getTime();
	String dayOfMonthOut = "";
	String monthOut= "";
	String yearOut= "";
	
	if(dayOfMonth < 10) dayOfMonthOut = "0" + (String)dayOfMonth;
	else dayOfMonthOut = (String)dayOfMonth;

	if(month < 10) monthOut = "0" + (String)month;
	else monthOut = (String)month;
	
	if(year < 10) yearOut = "0" + (String)year;
	else yearOut = (String)year;
	
	return yearOut + "/" + monthOut + "/" + dayOfMonthOut;
}
String easy2Time::nowFile(){
	getTime();
	String hourOut= "";
	String minuteOut= "";
	String secondOut= "";
	
	if(hour < 10) hourOut = "0" + (String)hour;
	else hourOut = (String)hour;
	
	if(minute < 10) minuteOut = "0" + (String)minute;
	else minuteOut = (String)minute;
	
	if(second < 10) secondOut = "0" + (String)second;
	else secondOut = (String)second;
	
	return hourOut + minuteOut + secondOut;
}

void easy2Time::setTimeAPP(uint8_t _day, uint8_t _month, uint8_t _year, uint8_t _hour, uint8_t _minute, uint8_t _second){
		// assign variables
	hour = _hour;
	minute = _minute;
	second = _second;
	year = _year;
	month = _month;
	dayOfMonth = _day;
	
	setTime();
	getTime();
}
void easy2Time::setTime()
{
	rtc.adjust(DateTime(year, month, dayOfMonth, hour, minute,second));
	getTime();
}

void easy2Time::getTime()
{
	DateTime now = rtc.now();
	hour = now.hour();
	minute = now.minute();
	second = now.second();
	year = now.year();
	month = now.month();
	dayOfMonth = now.day();
	
	
	
	
	
	
	
}