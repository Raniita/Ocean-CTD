#ifndef EasyLogger_h
#define EasyLogger_h
#include "Arduino.h"
#include <SPI.h>

class EasyLogger
{
public:
EasyLogger(byte chipSelect); ~EasyLogger();
void begin(char folderName[6], char fileName[6]),  add(char _incoming), openWrite(), closeWrite(); 
String info(), getFileName(), getFullPath(), getFolderName();
long getWrites(), getSize();
String fileName, folderName;


private:
File dataFile;

long sizeValue;
byte chipSelect;
};
#endif
