
/** Auto Stage One **/

float mx, my;
float distance_per_count = PI*12.0/1668;
float radians_per_count = distance_per_count/50.0;
float motor_heading = PI/2.0;
long int prev_left_counts, prev_right_counts;

void Auto_Stage_One(){
  while(1){
    //  prevmillis = millis();
    if( (encoder_motor1 < distances[path_phase]) && (encoder_motor2 < distances[path_phase]) ){
      Move_Turret();
      Move_Servo();
      Query_Launchpad();
      if (pid_enable){
        PID_Adjust();
      }
      Serial_Print();
///      delay(delay_long);
      Check_Abort();
    } else {
      /*LCD.setCursor(0,1);
      LCD.print("Path phase:");
      LCD.print(path_phase);*/
      path_phase++;
      Transform[path_phase]();
      if(stage_one_complete)
        break;
    }
    //  LAPTOP.println(millis() - prevmillis);
  }
}

void Pick_Leaves(){
  LAPTOP.println("Going to pick up the leaves.");
  pid_enable = false;
  Move_Forward(30, 30);
  encoder_turret = 0;
  turret_motor.Control(Check_Mirror(BCK,FWD),255);
  actuation_phase++;
  servo1.SetTargetAngle(2);
  servo2.SetTargetAngle(2);
}

void Accelerate_Bot(){
  LAPTOP.println("Stopping to pick up leaves");
  Motors_Brake(VSOFTBRAKE, VSOFTBRAKE);
  while(encoder_turret<TURRET_ANG1){
    Move_Servo();
  }
  turret_motor.Brake(0);
  Actuate_High(V_PISTON);
  delay(1000);
  Actuate_Low(V_PISTON);
  LAPTOP.println("Picked up leaves");
  Toggle_Wait();
  Launchpad_Reset();
  encoder_motor1 = encoder_motor2 = 0;
  Serial_Print();
  pid_enable = true;
  base_pwm = minimum_pwm;
  Move_Forward(minimum_pwm, minimum_pwm);
  turret_motor.Control(Check_Mirror(BCK,FWD), 220);
  actuation_phase++;
  servo2.SetTargetAngle(3);
  LAPTOP.println("Acceleration begun");

  while(base_pwm<maximum_pwm){
    base_pwm += acceleration;
    if(base_pwm == 36){
      Serial.println("\n PID begun \n");    /// indicate start of PID
      encoder_motor1 = encoder_motor2;
      pid.SetOutputLimits(-35,35);
    }
    if(base_pwm>35){
      PID_Adjust();
    }
    Query_Launchpad();
    Move_Turret();
    Move_Servo();
    delay(acceleration_delay);
    Serial_Print();
    Check_Abort();
  }
  
  LAPTOP.println("Acceleration completed.");
  cons_kp = 1.5;
  pid.SetTunings(cons_kp,cons_ki,cons_kd);
  pid.SetOutputLimits(-45,45);
}

void Decelerate_Bot(){
  LAPTOP.println("Deceleration begun!");

  while(base_pwm>slowdown_pwm){
    base_pwm -= deceleration;
    Query_Launchpad();
    PID_Adjust();
    delay(deceleration_delay);
    Move_Turret();
    Move_Servo();
    Serial_Print();
    Check_Abort();
  }
  
  LAPTOP.println("Deceleration done");
  base_pwm = slowdown_pwm;
  pid.SetOutputLimits(-35,35);
}

void Drop_First_Leaf(){
  Actuate_High(Check_Mirror(LEFT_VG,RIGHT_VG));
  LAPTOP.println("First leaf dropped");
  if(encoder_turret<TURRET_ANG2)
    LAPTOP.println("but turret didn\'t rotate enough");
  servo2.SetTargetAngle(2);
  turret_motor.Control(Check_Mirror(BCK,FWD), 255);
  actuation_phase++;
}

void Drop_Second_Leaf(){
  Motors_Brake(HARDBRAKE,HARDBRAKE);
  LAPTOP.println("Waiting for turret to turn");
  while(encoder_turret<TURRET_ANG3);
  turret_motor.Brake(0);
  delay(100);
  Query_Launchpad();
  Serial_Print();
  Actuate_High(MIDDLE_VG);
  LAPTOP.println("Middle leaf dropped");
  //Serial_Wait();
  LAPTOP.println("Moving forward slightly");
  Move_Forward(20,20);
  while(S2.Low()&&S3.Low());
  LAPTOP.println("Line detected");
  pid_enable = false;
  Launchpad_Reset();
  encoder_motor1 = 0; encoder_motor2 = 0;
  turret_motor.Control(Check_Mirror(FWD,BCK), 255);
  actuation_phase++;
}

void Soft_Turn(){
  LAPTOP.println("Going into soft turn");
  motor1.pwm(150);
  motor2.Brake(HARDBRAKE);
  pid_type = SOFT_TURN_PIDL;
  pid_enable = true;
  Set_Turn(distances[path_phase]);
  Launchpad_Reset();
  encoder_motor1 = 0; encoder_motor2 = 0;
}

void Auto_Stage_One_Complete(){
  Motors_Brake(HARDBRAKE,HARDBRAKE);
  LAPTOP.println("Waiting for turret to turn");
  while(encoder_turret<TURRET_ANG4){
    Move_Servo();
  }
  turret_motor.Brake(0);
  if( strategy == 2 ){
    // Drop third leaf
  }
  LAPTOP.println("Stopping after turn to drop the third leaf.");  
  Launchpad_Reset();
  encoder_motor1 = encoder_motor2 = 0;
  LAPTOP.println("Auto Stage One Complete. Beginning to follow line... ");
  stage_one_complete = true;
}

void Set_Turn(float ang){
  setpoint = ang/360.0*encoder_count;
  cons_kp = 255.0/setpoint;
  distances[path_phase] = setpoint;
  pid.SetTunings(cons_kp, cons_ki, cons_kd);
  pid.SetOutputLimits(0,255);  
}

void PID_Adjust(){
  
  if( pid_type == STRAIGHT_LINE_PID ){
   /** straight line PID **/
//    calculate_x();
    input = (encoder_motor1 - encoder_motor2);
//    if(input>-15&&input<15)
//      pid.SetTunings(cons_kp, cons_ki, cons_kd);
    pid.Compute();
    motor1.pwm(base_pwm + output);
    motor2.pwm(base_pwm - output);
    
  }else if( pid_type == SOFT_TURN_PIDL ){
    /** soft turn PID **/
    input = encoder_motor1;
    pid.Compute();
    if(output>35){
      motor1.pwm(output);
    }else{
      motor1.pwm(20);
    }
    if(S1.High()||S2.High()||S3.High()||S4.High()){
      LAPTOP.println("\n DETECTED LINE");
      line_detected = true;
 //     path_phase++;
//      Transform[path_phase]();
    }
  }else if( pid_type == SOFT_TURN_PIDR ){
    /** soft turn PID **/
    input = encoder_motor2;
    pid.Compute();
    if(output>45){
      motor2.pwm(output);
    }else{
      motor2.pwm(15);
    }
    if(S1.High()||S2.High()||S3.High()||S4.High()){
      LAPTOP.println("\n DETECTED LINE");
      line_detected = true;
 //     path_phase++;
//      Transform[path_phase]();
    }
  }else if( pid_type == SOFT_TURN_PIDR_SLOW ){
    /** soft turn PID **/
    input = encoder_motor2;
    pid.Compute();
    if(output>150){
      motor2.pwm(150);
    }else if(output>45){
      motor2.pwm(output);
    }else{
      motor2.pwm(15);
    }
    if(S1.High()||S2.High()||S3.High()||S4.High()){
      LAPTOP.println("\n DETECTED LINE");
      line_detected = true;
 //     path_phase++;
//      Transform[path_phase]();
    }
  }else if( pid_type == SOFT_TURN_PIDL_SLOW ){
    /** soft turn PID **/
    input = encoder_motor1;
    pid.Compute();
    if(output>150){
      motor1.pwm(150);
    }else if(output>45){
      motor1.pwm(output);
    }else{
      motor1.pwm(15);
    }
    if(S1.High()||S2.High()||S3.High()||S4.High()){
      LAPTOP.println("\n DETECTED LINE");
      line_detected = true;
 //     path_phase++;
//      Transform[path_phase]();
    }
  }
}

void Move_Servo(){
  servo1.Sweep(servo_speeds[path_phase]);
  servo2.Sweep(servo_speeds[path_phase]);
}

void Move_Turret(){
  switch(actuation_phase){
  case 1:
    // To pick up leaves
    if(encoder_turret>TURRET_ANG1){
      turret_motor.Brake(255);
    }
    break;
  case 2:
    // To drop first leaf
    if(encoder_turret>TURRET_ANG2){
      turret_motor.Brake(255);
    }
    break;
  case 3:
    // To drop second leaf
    if(encoder_turret>TURRET_ANG3){
      turret_motor.Brake(255);
    }
    break;
  case 4:
    // To drop third leaf
    if(encoder_turret>TURRET_ANG4){
      turret_motor.Brake(255);
    }
    break;
  }
}

float calculate_x(){
    int delta_left = encoder_motor1 - prev_left_counts, delta_right = encoder_motor2 - prev_right_counts;
    float delta_distance = (float )(delta_left + delta_right)*0.5*distance_per_count;
    float delta_heading = (float )(delta_left - delta_right)*radians_per_count;
    mx += delta_distance*(float)cos(motor_heading);
    motor_heading += delta_heading;
    if(motor_heading > PI)
      motor_heading -= TWO_PI;
    else if(motor_heading <= -PI)
      motor_heading += TWO_PI;
    prev_left_counts = encoder_motor1;
    prev_right_counts = encoder_motor2;
}
