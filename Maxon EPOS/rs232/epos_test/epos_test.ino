#include <epos2_v2.h>
EPOS2 e, e2;
int32_t t;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Begun");
  Serial.println("initializing E");
  e.init(&Serial1, 7);
  init(e);
//  Serial.println("initializing E");
//  e2.init(&Serial2, 3);
//  init(e2);
}

void loop() {
  // put your main code here, to run repeatedly: 
  Serial.println("Starting E");
  t = e.set(CONTROLWORD, 0x0F); // switch_on, enable_voltage, quickstop, enable_operation
  if(t) {
    Serial.println("ERROR: Couldn't start epos2.");
  }
  INT_ERROR err = 0;
  INT_CONTROLWORD a = e.get(CONTROLWORD, &err);
  Serial.println(a);
  Serial.println(err);
/*  Serial.println("Starting E2");
  t = e.set(CONTROLWORD, 0x0F); // switch_on, enable_voltage, quickstop, enable_operation
  if(t) {
    Serial.println("ERROR: Couldn't start epos2 2.");
  }
  err = -1;
  a = e.get(CONTROLWORD, &err);
  Serial.println(a);
  Serial.println(err);
*/}

void init(EPOS2 a) {  
  t = a.set(CONTROLWORD, 0x06);
   if(t) {
    Serial.print("ERROR: Couldn't switch off epos2. -- ");
    Serial.println(t);
  }  
  t = a.set(MOTOR_TYPE, DC_MOTOR);
  if(t) {
    Serial.print("WARNING : Couldn't set motor to DC -- ");
    Serial.println(t);
  }
  t = a.set(RS232_FRAME_TIMEOUT, 100);
  if(t) {
    Serial.print("WARNING : Couldn't set rs232 timeout -- ");
    Serial.println(t);
  }
  t = a.set(MAX_ACCELERATION, 4000);
  if(t) {
    Serial.print("WARNING : Couldn't set max acceleration -- ");
    Serial.println(t);
  }
  t = a.set(MAXIMAL_PROFILE_VELOCITY, 4000);
  if(t) {
    Serial.print("WARNING : Couldn't set maximal profile velocity -- ");
    Serial.println(t);
  }
  t = a.set(MAXIMAL_FOLLOWING_ERROR, 0000);
  if(t) {
    Serial.print("WARNING : Couldn't set maximal following error -- ");
    Serial.println(t);
  }
  t = a.set(GEAR_RATIO_NUMERATOR, 1);
  if(t) {
    Serial.print("WARNING : Couldn't set Gear ratio Numerator -- ");
    Serial.println(t);
  }
  t = a.set(GEAR_RATIO_DENOMINATOR, 21);
  if(t) {
    Serial.print("WARNING : Couldn't set Gear ratio Numerator");
    Serial.println(t);
  }
}
