#include <my_epos2.h>

int t;
EPOS2 e;
EPOS2 e2;

void setup() {
  Serial.begin(115200);
  Serial.println("Begun");
  e.init(&Serial1, 7);
  e2.init(&Serial2, 3);
}

void loop() {
  int32_t t;
  Serial.print("Num : ");
  Serial.println(e.get_gear_ratio_numerator());
  Serial.print("Denom : ");
  Serial.println(e.get_gear_ratio_denominator());
  Serial.println("going to 30k ...");
  
  Serial.println( e.get_position_actual_value());
  e.enable();
  e.velocity(200 * 60);  
  e.velocity();
  Serial.println("vel set");

  Serial.println(e.get_velocity_actual_value());
  delay(3000);
  Serial.println("done");
  Serial.println( e.get_position_actual_value());
  while(1);

  // HALT code
  t = e.get_controlword_bits();
  Serial.print("E - control word 1 : ");
  Serial.print(t, BIN);
  t = e.set_controlword(0x04);
  Serial.print(" --> Shutdown --> ");
  t = e.get_controlword_bits();
  Serial.println(t, BIN);

  t = e.get_statusword_bits();
  Serial.print("E - status word 5 : ");
  Serial.println(t, BIN);





  Serial.println("DONE");

  Serial.println();
  Serial.println();
  while(1);
}



