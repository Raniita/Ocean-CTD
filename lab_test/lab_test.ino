// Enrique 'Rani'. 25 Octubre 2019
//====================================================================
//============================= Librarys =============================
//====================================================================
#include <cyclopSensor.h>
#include <avr/pgmspace.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

#define BAUDRATE 115200

// Plateado
#define cdom_type "cdom"
#define cdom_id "1"
#define cdom_read A0
#define cdom_x10 9
#define cdom_x100 8
#define cdom_maxPPB 1500

// Negro
#define phy_type "phy"
#define phy_id "2"
#define phy_read A1
#define phy_x10 7
#define phy_x100 6
#define phy_maxPPB 750

// Amarillo
#define chl_type "chl"
#define chl_id "3"
#define chl_read A2
#define chl_x10 5
#define chl_x100 3
#define chl_maxPPB 500

cyclopSensor cdom(cdom_id, cdom_read, cdom_x10, cdom_x100);
cyclopSensor phy(phy_id, phy_read, phy_x10, phy_x100);
cyclopSensor chl(chl_id, chl_read, chl_x10, chl_x100);

void setup(){
  Serial.begin(BAUDRATE);
	Serial.println(F("Initialized serial conection"));
  delay(1000);

  Serial.println(F("Cyclops configuration: "));
  cdom.setMaxPPB(cdom_maxPPB);
  Serial.println("CDOM at PPB: " + (String) cdom.getMaxPPB());
  phy.setMaxPPB(phy_maxPPB);
  Serial.println("PHY at PPB: " + (String) phy.getMaxPPB());
  chl.setMaxPPB(chl_maxPPB);
  
  Serial.println("CDOM Gain: x" + (String) cdom.getGain());
  Serial.println("Phy Gain: x" + (String) phy.getGain());
  Serial.println("CHL Gain: x" + (String) chl.getGain());

  // Antes de entrar al loop
  Serial.println(" ");
  delay(2000);
}

void loop(){
  delay(3500);

  // sample() --> Version antigua de tomar medidas. Valor directo en PPB
  // measure() --> Mi implementacion. Valor en mV
  Serial.println(F("\nMeasuring..."));
	String cdom_measure = (String) cdom.sample();
  String cdom_mv = (String) cdom.measure();
	String cdom_gain = (String) cdom.getGain();
	String phy_measure = (String) phy.sample();
  String phy_mv = (String) phy.measure();
  String phy_gain = (String) phy.getGain();
  String chl_measure = (String) chl.sample();
  String chl_mv = (String) chl.measure();
  String chl_gain = (String) chl.getGain();

	Serial.println("CDOM => x" + cdom_gain + " value: " + cdom_measure + " mV: " + cdom_mv);
  Serial.println("Phyto => x" + phy_gain + " value: " + phy_measure + " mV: " + phy_mv);
  Serial.println("CHL => x" + chl_gain + " value: " + chl_measure + " mV: " + chl_mv);

	// Sleep arduino
	//LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);	
}
