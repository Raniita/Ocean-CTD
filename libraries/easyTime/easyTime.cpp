#include <Wire.h>
#include "easyTime.h"

easyTime::easyTime(uint8_t _easyTime_I2C_ADDRESS){
	easyTime_I2C_ADDRESS = _easyTime_I2C_ADDRESS;
 
 
 }

uint8_t easyTime::decToBcd(uint8_t val)
{
	return ( (val/10*16) + (val%10) );
}

//Convert binary coded decimal to normal decimal numbers
uint8_t easyTime::bcdToDec(uint8_t val)
{
	return ( (val/16*10) + (val%16) );
}

void easyTime::begin()
{
	Wire.begin();
}

/****************************************************************/
/*Function: Read time and date from RTC	*/
void easyTime::getTime()
{
    // Reset the register pointer
	Wire.beginTransmission(easyTime_I2C_ADDRESS);
	Wire.write((uint8_t)0x00);
	Wire.endTransmission();  
	Wire.requestFrom(easyTime_I2C_ADDRESS, 7);
	// A few of these need masks because certain bits are control bits
	second	   = bcdToDec(Wire.read() & 0x7f);
	minute	   = bcdToDec(Wire.read());
	hour	   = bcdToDec(Wire.read() & 0x3f);// Need to change this if 12 hour am/pm
	dayOfWeek  = bcdToDec(Wire.read());
	dayOfMonth = bcdToDec(Wire.read());
	month      = bcdToDec(Wire.read());
	year	   = bcdToDec(Wire.read());
}
/*******************************************************************/
/*Frunction: Write the time that includes the date to the RTC chip */
void easyTime::setTime()
{
	Wire.beginTransmission(easyTime_I2C_ADDRESS);
	Wire.write((uint8_t)0x00);
	Wire.write(decToBcd(second));// 0 to bit 7 starts the clock
	Wire.write(decToBcd(minute));
	Wire.write(decToBcd(hour));  // If you want 12 hour am/pm you need to set bit 6 
	Wire.write(decToBcd(dayOfWeek));
	Wire.write(decToBcd(dayOfMonth));
	Wire.write(decToBcd(month));
	Wire.write(decToBcd(year));
	Wire.endTransmission();
}
void easyTime::fillByHMS(uint8_t _hour, uint8_t _minute, uint8_t _second)
{
	// assign variables
	hour = _hour;
	minute = _minute;
	second = _second;
}
void easyTime::fillByYMD(uint16_t _year, uint8_t _month, uint8_t _day)
{
	year = _year-2000;
	month = _month;
	dayOfMonth = _day;
}
void easyTime::fillDayOfWeek(uint8_t _dow)
{
	dayOfWeek = _dow;
}

String easyTime::nowIs(){
	
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

String easyTime::todayDir(){
	
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
String easyTime::nowFile(){
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

void easyTime::setTimeAPP(uint8_t _day, uint8_t _month, uint8_t _year, uint8_t _hour, uint8_t _minute, uint8_t _second){
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

	
	
	
