
#include <epos2.h>

int t;
EPOS2 e(&Serial1, 3);
EPOS2 e2(&Serial2, 7);

void setup() {
  Serial.begin(115200);
  Serial.println("Begun");
}


void loop() {
  uint32_t t;
  epos2_frame f;

  t = e2.get_device_version();
  Serial.print("E2 - version : ");
  Serial.println(t, HEX);

  t = e2.get_device_type();
  Serial.print("E2 - type : ");
  Serial.println(t, HEX);
  
  t = e2.get_rs232_timeout();
  Serial.print("E2 - rs232 timeout : ");
  Serial.println(t, HEX);
  
  t = e.get_device_version();
  Serial.print("E - version : ");
  Serial.println(t, HEX);

  t = e.get_device_type();
  Serial.print("E - type : ");
  Serial.println(t, HEX);
  
  t = e.get_rs232_timeout();
  Serial.print("E - rs232 timeout : ");
  Serial.println(t, HEX);
  

  /*t = e2.send(get_frame(GET_DEVICE_VERSION));
  e2.get(&f);
  Serial.print("E - ver : ");
  Serial.println(t, HEX);
  
  t = e2.send(get_frame(GET_DEVICE_VERSION));
  e2.get(&f);
  Serial.print("E - ver : ");
  Serial.println(t, HEX);
  */
  
  Serial.println();
  Serial.println();
  while(1);
}


