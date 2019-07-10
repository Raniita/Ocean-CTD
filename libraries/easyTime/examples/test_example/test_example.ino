#include <easyTime.h>

easyTime reloj(0x68);


void setup(){
  reloj.begin();
  Serial.begin(9600);
  
reloj.setTimeAPP(25,12,6,4,13,3);
  
}

void loop(){

  Serial.println(reloj.nowIs());
  Serial.println(reloj.todayDir()); // fecha (ruta)
  
  Serial.println(reloj.nowFile()); //hora (nombre archivo)

  Serial.println(reloj.todayDir() + "/" + reloj.nowFile() + ".csv");
  delay(2000);
}

