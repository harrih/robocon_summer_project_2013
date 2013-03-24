
/** Reset Functions **/

void Turret_Reset(){
  char input;
  do{
    LAPTOP.println("Resetting Turret : c => Clockwise | a => Anticlockwise");
    input = Serial_Wait();
  }while(!Move_Turret_Dir(input));
  LAPTOP.println("Turret reset complete");  
}

boolean Move_Turret_Dir(char sense){
  turret_motor.pwm(TURRET_PWM);
  if(sense == 'c') {
    turret_motor.Control(FWD);
  }else if(sense == 'a' ){
    turret_motor.Control(BCK);
  }else{
    return false;
  }
  delay(200);

  int sensor_reading, detected_count = 0, reading_count = 0;
  while(turret_sensor.High()){
    Check_Abort();
  }
  
  turret_motor.Brake(255);
  return true;
}

void Parallelogram_Reset(){
  Actuate_High(GRIPPER);
  Parallelogram_Down();
  while(!Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_BOTTOM));
  Parallelogram_Stop();
  LAPTOP.println("Parallelogram reset done");
}

void Parallelogram_Reset_Custom(){
  char input;
  do{
    LAPTOP.println("Resetting Parallelogram : u => Rising up | d => Dropping down");
    input = Serial_Wait();
  }while(!Move_Parallelogram(input,1));
  LAPTOP.println("Parallelogram reset complete");  
}

boolean Move_Parallelogram(char sense, int num){
  if(sense == 'u'){
    Parallelogram_Up();
  }else if(sense == 'd'){
    Parallelogram_Down();
  }else
    return false;
  parallelogram_count = 0;
  while(parallelogram_count != num){
    Check_Abort();
  }
  Parallelogram_Stop();
}

void Turret_ISR(){
  encoder_turret++;
}

void Parallelogram_ISR(){
 parallelogram_count++;
}

void Parameters_Reset(){
  Query_Launchpad();
  Serial_Print();
  encoder_motor1 = encoder_motor2 = 0; 
  LAPTOP.println("Parameters have been reset");
}
