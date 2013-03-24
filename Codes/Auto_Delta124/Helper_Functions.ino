
/** Helper functions **/

int inline Check_Mirror(int a, int b){
  return mirror?a:b;
}

char Serial_Wait(){
  LAPTOP.println("Waiting for Serial Input ");
  while(!LAPTOP.available());
//    LAPTOP.println(analogRead(A2)); // for sharp
  return LAPTOP.read();
}

void Toggle_Wait(){
  if( do_toggle == false)
    return;
  int toggle_pin = 52;
  int static pin_state = digitalRead(toggle_pin);
  if(pin_state == HIGH){
    while(digitalRead(toggle_pin));
    delay(200);
    while(!digitalRead(toggle_pin));
    delay(200);
    while(digitalRead(toggle_pin));    
  }else{
    while(!digitalRead(toggle_pin));
    delay(200);
    while(digitalRead(toggle_pin));
  }
//  delay(1000);
}

void Toggle_Wait1(){
  int toggle_pin = 52;
  int static pin_state = digitalRead(toggle_pin);
  if(pin_state == HIGH){
    while(digitalRead(toggle_pin));
    delay(200);
    while(!digitalRead(toggle_pin));
    delay(200);
    while(digitalRead(toggle_pin));    
  }else{
    while(!digitalRead(toggle_pin));
    delay(200);
    while(digitalRead(toggle_pin));
  }
//  delay(1000);
}

void Abort(){
  Motors_Brake(255, 255);
  turret_motor.Brake(0);
  Parallelogram_Stop();
  LAPTOP.println("ABORTED!");
  while(1);
}

void Serial_Print(){
  LAPTOP.print("AVG Enc Val:"); LAPTOP.print((encoder_motor1+encoder_motor2)/2);  
  LAPTOP.print("\tenct: "); LAPTOP.print(encoder_turret); LAPTOP.print(" Turret Target:"); LAPTOP.print(encoder_turret_target);
  /*LAPTOP.print("\tcsl: ");
  LAPTOP.print(servo1.GetAngle());
  LAPTOP.print("\tcsr: ");
  LAPTOP.print(servo2.GetAngle());
  LAPTOP.print("\tsharp: ");
  LAPTOP.print(analogRead(SHARP_SENSOR_PIN));*/
  if(pid_enable){
    LAPTOP.print("\t  Current Diff: ");
    LAPTOP.print(input - setpoint);
    LAPTOP.print("\t PID Output: ");
    LAPTOP.print(output);
    if( pid_type == STRAIGHT_LINE_PID ){
      LAPTOP.print("\t Left PWM: ");
      LAPTOP.print(base_pwm + output);
      LAPTOP.print("\t Right PWM: ");
      LAPTOP.print(base_pwm - output);  
      LAPTOP.print("\tx: ");
      LAPTOP.println(mx);
    }else{
      LAPTOP.print("\t Left PWM: ");
      LAPTOP.println(output);
    }
  }else{
    LAPTOP.print("\n");
  }
}

void Serial_Print_Sensors(){
  if(S1.High()||S2.High()||S3.High()||S4.High()){
  LAPTOP.print(S1.High()); 
  LAPTOP.print(S2.High()); 
  LAPTOP.print(S3.High());
  LAPTOP.println(S4.High());
  }
}

void Handshake_Launchpad(){
  char msg = '@';
  while(msg != '#'){
    msg = '@';
    LAUNCHPAD.write(msg);
    while(!LAUNCHPAD.available());
    msg = LAUNCHPAD.read();
    delay(500);    
  }
  LAPTOP.println("Handshake completed");
}

// Function for stopping parallelogram at top position. Parallelogram will stop only if trip is switched else will continue to try to rise up.
int Parallelogram_Tripped(){
   if(Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_TOP)){
     Parallelogram_Stop();
     return 1;
   }else{
     Parallelogram_Up(); // Case added to prevent / counter sagging
     return 0;
   }
}

int Parallelogram_Reached(int count){
  LAPTOP.print("PARALLELOGRAM COUNT:");
  LAPTOP.println(parallelogram_count);
  if( parallelogram_count >= count ){
    if(  parallelogram_sensor.Low() ){
      Parallelogram_Stop();
      return 1;
    }   
  }
  return 0;
}

int Turret_Reached(int count){
  LAPTOP.print("TURRET COUNT:");
  LAPTOP.println(turret_count);
  if( turret_count >= count ){
    turret_motor.Brake(255);
    return 1;
  }
  return 0;
}

void Check_Motors(int pwm){
  while(1){
    motor1.Control(FWD, pwm);
    motor2.Brake(255);
//    motor2.Control(FWD, pwm);
    Check_Abort();
    Query_Launchpad();
    Serial_Print();
  }
}

void Check_TSOP(){
  while(!LAPTOP.available()) {
    LAPTOP.print(4);
    LAPTOP.print(" ");
    LAPTOP.print(digitalRead(4));
    LAPTOP.print("\t");
    LAPTOP.print(5);
    LAPTOP.print(" ");
    LAPTOP.print(digitalRead(5)); 
    
    LAPTOP.print("\t");
    LAPTOP.print("Communication ");
    if(digitalRead(4) == LOW || digitalRead(5) == LOW)
      LAPTOP.println("Yes");
    else
      LAPTOP.println("No");
    delay(100);
  }
  LAPTOP.read();
}

int Trip_Switch(int pin){
  if(!digitalRead(pin)){
    return 0;
  }else{
    delay(10);
    if(digitalRead(pin))
      return 1;
    else
      return 0;    
  }
  
}

int Read_Sharp(){
  int reading = 0;
  for(int i =0; i<10; i++)
    reading += analogRead(SHARP_SENSOR_PIN);
  return reading/10;
}
