// Enrique 'Rani'. 22 Julio 2019
//====================================================================
//============================= Librarys =============================
//====================================================================
#include <cyclopSensor.h>
#include <avr/pgmspace.h>
#include <LowPower.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

/* Configuracion del puerto serie
 * Baudios -> 9600 o otro diferente
 * 
 */
#define BAUDRATE 9600

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
// SENSOR 1: CDOM
#define sensor_name_1 "CDOM"
#define id_1 1
#define sn_1 21180135
#define readPin_1 A0
#define x10_1 6
#define x100_1 7
#define maxPPB_1 1500

// SENSOR 2: Phycoerythin
#define sensor_name_2 "PHY"
#define id_2 2 
#define sn_2 21180134
#define readPin_2 A1
#define x10_2 8
#define x100_2 9
#define maxPPB_2 750

cyclopSensor cdom(id_1, sn_1, readPin_1, x10_1, x100_1, maxPPB_1);
cyclopSensor phy(id_2, sn_2, readPin_2, x10_2, x100_2, maxPPB_2);

void setup(){
	Serial.begin(BAUDRATE);
	Serial.println(F("Initialized serial conection \n"));
}

void loop(){
	// Taking measures
	String cdom_measure = (String) cdom.measure();
	String phy_measure = (String) phy.measure();	
	Serial.println("Value of CDOM: " + cdom_measure);
	Serial.println("Value of PHY: " + phy_measure);
	
	// Sleep arduino
	LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);	
}
