// Enrique 'Rani'. 25 Octubre 2019
//====================================================================
//============================= Librarys =============================
//====================================================================
#include <cyclopSensor.h>
#include <avr/pgmspace.h>
//#include <LowPower.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

/* Configuracion del puerto serie
 * Baudios -> 9600 o otro diferente
 * 
 */
#define BAUDRATE 115200

// Configuracion de los sensores
/*
 * Formato del constructor
 * sensor(id,sn,readPin,x10pin,x100pin,maxPPB)
 * 
 * id -> ID del sensor
 * sn -> Numero del serie del sensor
 * readPin -> Pin de lectura
 * x10pin -> Pin Gain x10
 * x100pin -> Pin Gain x100
 * maxPPB -> max de PPB por sensor
 */
// Sensor 1: CDOM
#define cdom_type "cdom"
#define cdom_id 1
#define cdom_readPin A0
#define cdom_x10Pin 6
#define cdom_x100Pin 7
#define cdom_maxPPB 1500

// Sensor 2: Phycoerythin
#define phy_type "phy"
#define phy_id 2 
#define phy_readPin A1
#define phy_x10Pin 8
#define phy_x100Pin 9
#define phy_maxPPB 750

cyclopSensor cdom(cdom_id, cdom_readPin, cdom_x10Pin, cdom_x100Pin);
cyclopSensor phy(phy_id, phy_readPin, phy_x10Pin, phy_x100Pin);

void setup(){
  // Configuramos el puerto serie
  Serial.begin(BAUDRATE);
	Serial.println(F("Initialized serial conection"));
  delay(50);

  Serial.println(F("Cyclops configuration: "));
  // Configuramos los sensores
  cdom.setMaxPPB(cdom_maxPPB);
  Serial.println("CDOM at PPB: " + (String) cdom.getMaxPPB());
  phy.setMaxPPB(phy_maxPPB);
  Serial.println("PHY at PPB: " + (String) phy.getMaxPPB());

  Serial.println("CDOM Gain: x" + (String) cdom.getGain());
  Serial.println("Phy Gain: x" + (String) phy.getGain());

  // Antes de entrar al loop
  Serial.println(" ");
  delay(2000);
}

void loop(){
  delay(1500);

  // Taking measures
  Serial.println(F("\nMeasuring..."));
  // sample() --> Version antigua de tomar medidas. Valor directo
  // measure() --> Mi implementacion. No da el valor directo
	String cdom_measure = (String) cdom.sample();
	String cdom_gain = (String) cdom.getGain();
	String phy_measure = (String) phy.sample();
  String phy_gain = (String) phy.getGain();	

  // Printing values on Serial
	Serial.println("CDOM => x" + cdom_gain + " value: " + cdom_measure);
  Serial.println("Phyto => x" + phy_gain + " value: " + phy_measure);

	// Sleep arduino
	//LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);	
}
