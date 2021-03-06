// Enrique 'Rani'. 16 Julio 2019
//====================================================================
//============================= Librarys =============================
//====================================================================
#include <mySensor.h>
#include <easy2Time.h>
#include <SPI.h>
#include <SD.h>
#include <avr/pgmspace.h>
// Probamos la libreria de interrupts del WDT
#include <LowPower.h>

//====================================================================
//============================= Configurations =======================
//====================================================================

/* Configuracion del puerto serie
 * Baudios -> 9600 o otro diferente
 * 
 */
#define BAUDRATE 9600

// Variables
#define chipSelect 10 // Algo para la SD
int vbat;
#define vbatpin A2

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
#define sensor_1Name "CDOM"
#define id_s 1
#define sn_s 21180135
#define readPin_s A0
#define x10pin_s 6
#define x100pin_s 7
#define maxPPB_s 1500

// SENSOR 2: Phycoerythin
#define sensor_2Name "PHY"
#define id_s2 2 
#define sn_s2 21180134
#define readPin_s2 A1
#define x10pin_s2 8
#define x100pin_s2 9
#define maxPPB_s2 750

mySensor sensor1(id_s, sn_s, readPin_s, x10pin_s, x100pin_s, maxPPB_s);
mySensor sensor2(id_s2, sn_s2, readPin_s2, x10pin_s2, x100pin_s2, maxPPB_s2);

/*
 * Documentacion de la shield:
 * https://learn.adafruit.com/adafruit-data-logger-shield/using-the-real-time-clock
 * 
 * Idea:
 *  - Cambiar este reloj a:
 *  https://aprendiendoarduino.wordpress.com/2016/11/16/arduino-sleep-mode/
 *  
 * El 0x68 es la dir I2C del RTC
 */
easy2Time reloj(0x68);

// Configuracion SD
String folderName;
String fileName;

char option= 'm';

void setup(){
  // Configuramos el serial
  Serial.begin(BAUDRATE);
  Serial.print(F("Initializated serial conection\n"));

  // Configuramos el RTC
  reloj.begin();

  // Configuramos la SD
  if (!SD.begin(chipSelect)) {
    Serial.println(F("0X01 - CARD IS NOT PRESENT, HALTED, PLEASE CHECK AND REBOOT"));
    // Esperamos hasta que la SD este funcional.
    while(true);
  }
}

void loop(){
  /*
   * Ideas:
   * Trabajar con interrupts del RTC para captar valores:
   * Cada vez que salta un interrupt:
   *  - Mandamos por Serial la hora
   *  - Obtenemos los datos de cada sensor
   *  - Comprobamos que las medidas estan dentro de sus valores de ganacia, sino se ajusta dicha ganancia
   *  - Mandamos o guardamos dichos valores
   * Con esta solucion lo que obtenemos son mejoras en consumo de bateria, y es mucho más especifico para la captacion de datos.
   * Podemos hacerlo directamente con el WDT timer, hay que revisar esto.
   */
   
  if(option == 'm') menu();
  if(option == 's') sampling();
  if(option == 't') setTime();
  if(option == 'g') getTime();
  if(option == 'v') getVbat();
  if(option == 'f') getSD();

  /*
   * Para dormir el Arduino 8s
   */
   //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void start(){ // este metodo genera un fichero nuevo
  folderName = "/" +reloj.todayDir()  + "/" + reloj.nowFile() + "/";
  fileName = "log.csv";
  SDbegin();

  //header();
  //dataFile.print(F("Date\tTime\t"));
  //dataFile.print(sensor_1Name);
  //dataFile.print(F("\t"));
  //dataFile.print(sensor_2Name);
  //dataFile.print(F("\n"));
  //dataFile.print(F("Date\tTime\t"));
  Serial.print((F("Date\tTime\tCDOM-ppb\tCDOM-gain\tCDOM-mv\tPHY-ppb\tPHY-gain\tPHY-mv\n")));  
 }

void getVbat(){
  Serial.print("$bat$");
  Serial.print((String)map(analogRead(A2), 0, 1023, 0, 14167) + " mV");
  Serial.print("%");
  if(option == 'v') option = 'm';
}

void getSD(){
  File dataFile = SD.open(folderName+fileName,  FILE_WRITE); //inicio el fichero
  Serial.print("$sd$");
  
  if(dataFile.size() > 1024){
    Serial.print((String)(dataFile.size()/1024) + " kB");
    } else {
      Serial.print((String)(dataFile.size()) + " Bytes");
    } 
  
  Serial.print("%");
  dataFile.close();
  if(option == 'f') option = 'm';
}

void setTime(){
  delay(300);
  reloj.dayOfMonth = Serial.read();
  reloj.month = Serial.read();
  reloj.year = Serial.read();
  reloj.hour = Serial.read();
  reloj.minute = Serial.read();
  reloj.second = Serial.read();
  reloj.setTime();
  option = 'm';
//  Serial.print(F(" Se ha recibido una sincronización horaria: "));
//  Serial.println(reloj.nowIs());
}

void getTime(){
  Serial.write(reloj.dayOfMonth);
  Serial.write(reloj.month);
  Serial.write(reloj.year);
  Serial.write(reloj.hour);
  Serial.write(reloj.minute);
  Serial.write(reloj.second);  
  option = 'm';
}


void sampling(){
  delay(650);
  double data1 = sensor1.readDouble();
  int gain1 = sensor1.readgain(); 
  //double mv1=sensor1.readmv();
 
  double data2 = sensor2.readDouble();
  int gain2 = sensor2.readgain();
  //double mv2=sensor2.readmv(); 
 

  Serial.print((String)data1);
  Serial.print(F(" "));
  Serial.print((String)gain1);
  Serial.print(F(" ")); 
  //Serial.print((String)mv1);
  Serial.print(F(" ")); 
  Serial.print((String)data2);
  Serial.print(F(" "));
  Serial.print((String)gain2);
  Serial.print(F(" "));
  //Serial.print((String)mv2);
  Serial.print(F(" ")); 

  //escribe el datos por puerto serie
  Serial.print(F("\n"));   

  if(Serial.available()){
    char receivedComand = Serial.read();
    if(receivedComand == 'h'){
      option = 'm';
    } else if(receivedComand == 'v') {
      getVbat();
      } else if(receivedComand == 'f') {
      getSD();
      }
  }
}

void menu(){
  if(Serial.available()){
    switch(Serial.read()){
      case 't': option = 't';
      break;
      case 's': 
        option = 's';
        // Ejecuto el script de creacion de un nuevo log!
        start(); 
      break;
      case 'g': option = 'g';
      break;
      case 'v': option = 'v';
      break;
       case 'f': option = 'f';
      break;
    
    }
  }
}

void SDbegin(){
  //generamos la carpetica
  SD.mkdir(folderName);
  //inicio el fichero 
  File dataFile = SD.open(folderName+fileName, FILE_WRITE); 
  // recojo de vuelta el nombre de fichero (por si se excede la longitud del nombre de archivo quede ok...)
  fileName = dataFile.name(); 
  dataFile.close();
}

void header(){
  //inicio el fichero
  File dataFile = SD.open(folderName+"info.txt",  FILE_WRITE); 
  dataFile.print(F("# @@@\n# nombre: "));
  dataFile.print(F(sensor_1Name));
  dataFile.print(F("\n# entrada analog: "));
  dataFile.print(readPin_s);
  dataFile.print(F("\n# num serie: "));
  dataFile.print(sn_s);
  dataFile.print(F("\n# Max PPB: "));
  dataFile.print(maxPPB_s);
  
  dataFile.print(F("\n# pin de entrada x10: "));
  dataFile.print(x10pin_s);
  
  dataFile.print(F("\n# pin de entrada x100: "));
  dataFile.print(x100pin_s);
 
  dataFile.print(F("\n# @@@\n"));

  dataFile.print(F("# @@@\n# nombre: "));
  dataFile.print(F(sensor_2Name));
  dataFile.print(F("\n# entrada analog: "));
  dataFile.print(readPin_s2);
  dataFile.print(F("\n# num serie: "));
  dataFile.print(sn_s2);
  dataFile.print(F("\n# Max PPB: "));
  dataFile.print(maxPPB_s2);

  dataFile.print(F("\n# pin de entrada x10: "));
  dataFile.print(x10pin_s2);
  
  dataFile.print(F("\n# pin de entrada x100: "));
  dataFile.print(x100pin_s2);
 
  dataFile.print(F("\n# @@@\n"));
  dataFile.close();
}
