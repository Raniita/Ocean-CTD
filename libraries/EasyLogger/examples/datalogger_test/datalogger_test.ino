#include <EasyLogger.h>
#define chipSelect 4

EasyLogger data("/TEST/", "log.txt", chipSelect);
int i = 0;

void setup()
{
Serial.begin(9600);
}

void loop()
{ 
  data.addLn("Hola mundo.");
  delay(2000);
  data.add("Hola");
  data.add(" k ");
  data.add("ase");
  data.addLn(", programa o k ase");

  Serial.println("info: " + data.info());
  Serial.println("Name: " + data.getFileName());
  Serial.println("Folder path: " + data.getFolderName());
  Serial.println("Full path: " + data.getFullPath());
  Serial.println("File size: " + (String)data.getSize() + "Bytes.");
}



