/*	
	José Luis Sanz Ruiz, UPCT.
	
	Librería para escribir un
	log en una SD con Arduino.
	
	Se puede invocar tantos objetos
	como sea necesario de forma
	simultanea, si necesita 
	escribir varios archivos al
	mismo tiempo.
	
	Si se repite el nombre del
	archivo se escribe a continuación.
	
	
	
	Peculiaridades, no se puede
	borrar nada, sólo añadir,
	es como un cuaderno de notas.
								*/


#include "Arduino.h"
#include "EasyLogger.h"
#include <SD.h>
#include <SPI.h>
#define ledPin 13

EasyLogger::EasyLogger(byte _chipSelect)
{
chipSelect = _chipSelect;
File dataFile();
	
}

EasyLogger::~EasyLogger(){
	
} 

void EasyLogger::begin(char _folderName[7], char _fileName[7]){
	
	if (!SD.begin(chipSelect)) {
    Serial.println("0X01 - CARD IS NOT PRESENT, HALTED, PLEASE CHECK AND REBOOT");
	while(true);
	}
  
  for(byte i = 0; i <= 8; i++){
	  fileName += _fileName[i];
	  folderName += _folderName[i];
  }
  
  
	SD.mkdir(folderName); //generamos la carpetica
	dataFile = SD.open(folderName+fileName, FILE_WRITE); //inicio el fichero
	fileName = dataFile.name(); // recojo de vuelta el nombre de fichero (por si se excede la longitud del nombre de archivo quede ok...)
	dataFile.close();
}

	
	void EasyLogger::openWrite(){
		dataFile = SD.open(folderName+fileName, FILE_WRITE); //inicio el fichero
		
	}
	
	void EasyLogger::closeWrite(){
		dataFile.close();
	}
		
		
	
void EasyLogger::add(char _incoming){
	dataFile.print(_incoming);
}
	

long EasyLogger::getSize(){
	dataFile = SD.open(folderName+fileName, FILE_WRITE);
	sizeValue = dataFile.size();
	dataFile.close();
	return sizeValue;
	
}

