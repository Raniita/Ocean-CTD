#include <cyclopSensor.h>

// Definimos el sensor
// En este caso utilizamos el sensor de phyto
#define sensor_name_1 "PHY"
#define sensor_id_1 1
#define sensor_sn_1 21180134
#define sensor_readPin_1 A1
#define sensor_x10_1 8
#define sensor_x100_1 9
#define sensor_maxPPB_1 750

// Declaramos el sensor
cyclopSensor phyto(sensor_id_1, sensor_sn_1, sensor_readPin_1, sensor_x10_1, sensor_x100_1, sensor_maxPPB_1);

void setup(){
	Serial.begin(9600);
	Serial.println("Sensor inizializado");
	Serial.println(phyto.measure());
}

void loop(){
	Serial.println("Valor medido: " + (String) phyto.measure());
}
