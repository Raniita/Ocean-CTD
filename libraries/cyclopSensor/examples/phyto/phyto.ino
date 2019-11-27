/*
	Example for phycoeritrina sensor
*/
#include <cyclopSensor.h>

#define phy_type "phy"
#define phy_id "2"
#define phy_read A1
#define phy_x10 7
#define phy_x100 6
#define phy_maxPPB 750

cyclopSensor phy(phy_id, phy_read, phy_x10, phy_x100);
String phy_measure, phy_mv, phy_gain;

void setup(){
	Serial.begin(9600);
	
	phy.setMaxPPB(phy_maxPPB);
	Serial.println("Sensor init.");
	delay(200);
	
	Serial.println("Phy Gain: x" + (String) phy.getGain());
}

void loop(){
    phy_measure = (String) phy.measure();
    phy_mv = (String) phy.getMV();
    phy_gain = (String) phy.getGain();
    
    Serial.println("PHY => x" + phy_gain + " value: " + phy_measure + " mV: " + phy_mv);
}
