
#include <epos2.h>

int t;
EPOS2 e(&Serial1, 7);
EPOS2 e2(&Serial2, 64);

void setup() {
  Serial.begin(115200);
  Serial.println("Begun");
}


void loop() {
  int t;
  t = e2.get_device_ver();
  Serial.print("E - ver : ");
  Serial.println(t, HEX);
  
  t = e2.get_device_type();
  Serial.print("E - type : ");
  Serial.println(t, HEX);
  
  t = e2.get_rs232_timeout();
  Serial.print("E - timeout : ");
  Serial.println(t, HEX);
  
  t = e2.get_device_ver();
  Serial.print("E - ver : ");
  Serial.println(t, HEX);
  
  t = e2.get_device_type();
  Serial.print("E2 - type : ");
  Serial.println(t, HEX);
  
  t = e2.get_rs232_timeout();
  Serial.print("E2 - type : ");
  Serial.println(t, HEX);
  
  Serial.println();
  Serial.println();
  while(1);
}


