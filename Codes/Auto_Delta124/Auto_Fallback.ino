
/** Auto Fallback **/

void Auto_Fallback(){
  while(!ps_complete){
    if(rg_nirma){
      Transform_RG[path_phase]();
      LAPTOP.println("Transform RG");
    }else if(omit_leaf1&&omit_leaf2 == 1){
      if(!omit_leaf3){
        Transform_3L_B1[path_phase]();
        LAPTOP.println("Transform 3L B1");
      }else if(bud_count == 0){
        Transform_NL_B1[path_phase]();
        LAPTOP.println("Transform NL B1");
      }else{
        Transform_NL_B23[path_phase]();
        LAPTOP.println("Transform NL B23");
      }
    }else{
      if(omit_bud1){
        Transform_12L_B23[path_phase]();
        LAPTOP.println("Transform 12L B23");
      }else{
        Transform_Fallback[path_phase]();
        LAPTOP.println("Transform");
      }
    }
    path_phase++;    
    Check_Abort();
  }
}

void To_Pick_LeavesF(){
  LAPTOP.println("Going to pick up the leaves.");
  Move_Forward(Check_Mirror(15,15), Check_Mirror(15,15));
  
  encoder_turret = 0;
  turret_motor.Control(Check_Mirror(BCK,FWD),255);
  encoder_turret_target = Check_Mirror(TURRET_ANG1MF, TURRET_ANG1F);

  servo1.SetTargetAngle(2);
  servo2.SetTargetAngle(2);

  Parallelogram_Up();
  while(encoder_motor1 < distance_leaves_pickup && encoder_motor2 < distance_leaves_pickup){
    Query_Launchpad();
    Move_TurretF();
    Move_Servo();
    Serial_Print();
    Check_Abort();
    if( parallelogram_sensor.High())
      Parallelogram_Stop();
  }
//  Motors_Brake(255,255);
//  Toggle_Wait();
//  Move_Forward(15,15);
}

void Pick_LeavesF(){
  LAPTOP.println("Stopping to pick up leaves");
  int threshold = Check_Mirror(300, 215);

/*  if(mirror){
    while(Read_Sharp() < threshold){
      LAPTOP.print("SHARP Check running"); LAPTOP.println(Read_Sharp());
  
      Query_Launchpad();
      Move_Servo();
      Move_TurretF();
      Serial_Print();
      if( parallelogram_sensor.High())
        Parallelogram_Stop();
    }
  }*/
  Motors_Brake(HARDBRAKE, HARDBRAKE);
  while(!parallelogram_sensor.High());
  Parallelogram_Stop();
  while(!Move_TurretF()){
    Move_Servo();
    Serial_Print();
  }
 
  if(omit_leaf1) 
    Actuate_High(LEFT_VG);
  if(omit_leaf3)
    Actuate_High(MIDDLE_VG);
  if(omit_leaf2)
    Actuate_High(RIGHT_VG);
  servo1.Middle();
  servo2.Middle();
  delay(1000);
  Actuate_High(V_PISTON);
  Parallelogram_Up();
  delay(50 + (omit_leaf1&&omit_leaf2&&!bud_count)*250);
  Parallelogram_Stop();
  delay(850 - (omit_leaf1&&omit_leaf2&&!bud_count)*250);
  Actuate_Low(V_PISTON);
  LAPTOP.println("Picked up leaves");
  //Toggle_Wait(); //Removed before first match; 
  delay(1000);
}

void Move_Straight_FastF(){
  prevmillis = millis();
//  Move_Forward(190,255);

  if(!omit_leaf1 || !omit_leaf2 || omit_leaf3){
    encoder_turret = 0;
    turret_motor.Control(Check_Mirror(FWD,BCK), 255);
    encoder_turret_target = Check_Mirror(TURRET_ANG2MF, TURRET_ANG2F);
  }
    
  LAPTOP.println("Moving forward fast");
  LAPTOP.println(distance_straight_line);

  servo1.SetTargetAngle(3);
  servo2.SetTargetAngle(3);

  pid_enable = true;
  pid_type = STRAIGHT_LINE_PID;
  base_pwm = 20;
  cons_kp = 1.5;
  pid.SetTunings(cons_kp,cons_ki,cons_kd);
  pid.SetOutputLimits(-15,15);
  Move_Forward(base_pwm, base_pwm);
  while(base_pwm < 150){
    if(base_pwm<70)
      base_pwm +=2;
    else
      base_pwm += 5;
    Query_Launchpad();
    Move_Turret_EncoderF();
    Move_Servo();
    PID_Adjust();
    Serial_Print();
    Check_Abort();
  }
  base_pwm = 210;
  pid.SetOutputLimits(-35,35);
  encoder_motor1 = encoder_motor2;
//  Move_Forward(190,255);
  //while(encoder_motor1 < distance_straight_line && encoder_motor2 < distance_straight_line){
  while((encoder_motor1 + encoder_motor2)/2< distance_straight_line){
    LAPTOP.println("ST line target:");    
    LAPTOP.print(distance_straight_line);
    Query_Launchpad();
    Move_Turret_EncoderF();
    Move_Servo();
    PID_Adjust();
    Serial_Print();
  }  
}
/*
void Accelerate_BotF(){
  //Toggle_Wait(); //Double toggle_wait because it was generally being bypassed. 
  Parameters_Reset();
  pid_enable = true;
  base_pwm = minimum_pwm;
  Move_Forward(minimum_pwm, minimum_pwm);
  encoder_turret = 0;
  turret_motor.Control(Check_Mirror(FWD,BCK), 220);
  encoder_turret_target = Check_Mirror(TURRET_ANG2MF, TURRET_ANG2F);
  LAPTOP.println("Acceleration begun");
  servo1.SetTargetAngle(3);
  servo2.SetTargetAngle(3);
  while(base_pwm < maximum_pwm){
    LAPTOP.println(encoder_motor1);
    base_pwm += acceleration;
    if(base_pwm == 36){
      LAPTOP.println("\n PID begun \n");    /// indicate start of PID
      encoder_motor1 = encoder_motor2;
      pid.SetOutputLimits(-35,35);
    }
    if(base_pwm>35){
      PID_Adjust();
    }
    Query_Launchpad();
    Move_TurretF();
    Move_Servo();
    delay(acceleration_delay);
    Serial_Print();
    Check_Abort();
  }

  LAPTOP.println("Acceleration completed.");
  cons_kp = 1.5;
  pid.SetTunings(cons_kp,cons_ki,cons_kd);
  pid.SetOutputLimits(-45,45);

  while(encoder_motor1 < (distances_fallback[path_phase]) && encoder_motor2 < (distances_fallback[path_phase])){
 //   while(encoder_motor1 < 3880 && encoder_motor2 < 3880){
   LAPTOP.println(encoder_motor1);
    Query_Launchpad();
    Move_TurretF();
    Move_Servo();
    Serial_Print();
    Check_Abort();
    PID_Adjust();
  }
}

void Decelerate_BotF(){

  LAPTOP.println("Deceleration begun!");

  while(base_pwm > slowdown_pwm){
    LAPTOP.println(encoder_motor1);
    base_pwm -= deceleration;
    Query_Launchpad();
    PID_Adjust();
    delay(deceleration_delay);
    Move_TurretF();
    Move_Servo();
    Serial_Print();
    Check_Abort();
  }

  LAPTOP.println("Deceleration done");
  base_pwm = slowdown_pwm;
  pid.SetOutputLimits(-35,35);

  while(encoder_motor1 < (distances_fallback[path_phase]) && encoder_motor2 < (distances_fallback[path_phase])){
  

  //while(encoder_motor1 < 5800 && encoder_motor2 < 5800){
    LAPTOP.println(encoder_motor1);
    Query_Launchpad();
    Move_TurretF();
    Move_Servo();
    Serial_Print();
    Check_Abort();
    PID_Adjust();
  }
}

void Detect_Line(){
  turret_motor.Brake(0);

  Motors_Brake(100, 100);
  delay(350);
  LAPTOP.println("Moving forward slightly");
  Move_Forward(20,20);
  while(S2.Low()&&S3.Low());
  LAPTOP.println("Line detected");
  
  Parameters_Reset();
  while(encoder_motor1 < distances_fallback[path_phase] && encoder_motor2 < distances_fallback[path_phase]){
    Query_Launchpad();
    Check_Abort();
    Move_TurretF();
  }
//  Motors_Brake(255,255);
//  delay(400);
}
*/
void Set_TurnF(float ang){
  setpoint = ang/360.0*encoder_count;
  cons_kp = 400.0/setpoint;
  distance_turn = setpoint;
  pid.SetTunings(cons_kp, cons_ki, cons_kd);
  pid.SetOutputLimits(0,255);  
}

void Turn_and_Align(int dir, int angle){
  LAPTOP.println("Going into soft turn");
//  Motors_Brake(255,255);
//  Toggle_Wait();
  Set_TurnF(angle);
  Parameters_Reset();
  if( dir == 1){
    pid_type = SOFT_TURN_PIDL;
  }else if( dir == 3){
    pid_type = SOFT_TURN_PIDR_SLOW;
  }else if( dir == 5){
    pid_type = SOFT_TURN_PIDL_SLOW;
  }else{
    pid_type = SOFT_TURN_PIDR;
  }

  while(encoder_motor1 < distance_turn && encoder_motor2 < distance_turn){
    LAPTOP.println("turn and align");
    line_detected = false;
    Query_Launchpad();
    Check_Abort();
//    if(turret_type == 1)
      Move_TurretF();
//    else if(turret_type == 2)
//      Move_Turret_EncoderF();
    PID_Adjust();
    Serial_Print();
    if(line_detected)
      if(distance_turn-encoder_motor1<750)//check and decide
        break;
    /*if(dir == 4 && encoder_motor1>distances_fallback[path_phase]-1500) { //check and decide
      line_detected = true;
      break;
    } */
     
  }
//  Motors_Brake(255,255);
//  delay(1000);
//  delay(100);//delay added to increase precision
  // code for correcting and aligning back on to the line
  LAPTOP.println("Aligned onto the line");
}


void Turn_and_Align1(int dir, int angle){
  LAPTOP.println("Going into soft turn");
//  Motors_Brake(255,255);
//  Toggle_Wait();
  Set_TurnF(angle);
  Parameters_Reset();
  if( dir == 1){
    pid_type = SOFT_TURN_PIDL;
  }else if( dir == 3){
    pid_type = SOFT_TURN_PIDR_SLOW;
  }else if( dir == 5){
    pid_type = SOFT_TURN_PIDL_SLOW;
  }else{
    pid_type = SOFT_TURN_PIDR;
  }

  while(encoder_motor1 < distance_turn && encoder_motor2 < distance_turn){
    line_detected = false;
    Query_Launchpad();
    Check_Abort();
//    if(turret_type == 1)
      Move_TurretF();
//    else if(turret_type == 2)
//      Move_Turret_EncoderF();
    PID_Adjust();
    Serial_Print();
    if(line_detected)
      if(distance_turn-encoder_motor1<distance_turn*0.2)//CHANGE MADE!!!!!!! 
        break;
    /*if(dir == 4 && encoder_motor1>distances_fallback[path_phase]-1500) { //check and decide
      line_detected = true;
      break;
    } */
     
  }
//  Motors_Brake(255,255);
//  delay(1000);
//  delay(100);//delay added to increase precision
  // code for correcting and aligning back on to the line
  LAPTOP.println("Aligned onto the line");
}


void First_LineFollow(){
//  servo1.SetTargetAngle(3);
//  servo2.SetTargetAngle(3);
  turret_motor.Brake(0);
  Motors_Brake(255,255);
  delay(800); //BRAKE DELAY FOR STRAIGHT PATH
//  servo1.Extend();
//  servo2.Extend();
  motor1.Control(FWD,0);
  Turn_and_Align1(1, Check_Mirror(60,60));//FIRST TURN
  Motors_Brake(255,255);
  delay(100); //BRAKE DELAY FOR FIRST PATH
//  Toggle_Wait(); // ADDED!!!! 
  while(S1.Low() && S2.Low() && S3.Low() && S4.Low() ){
    LAPTOP.println("C");
    Serial_Print();
    if(line_detected){
      motor1.Brake(255);
      motor2.Control(FWD,20);
    }else{
      motor2.Brake(255);
      motor1.Control(FWD,20);
    }
  }  
  LAPTOP.println("LineFollow to 1st Latitude");
  Parameters_Reset();
/*  while(analogRead(SHARP_SENSOR_PIN)<400){
    LineFollow34();
    Move_Servo();
    Move_TurretF();
    /*
    if(flag_turret == 1 || turret_sensor.Low()) {
      turret_motor.Brake(255);
      flag_turret = 1;
    }
    if(( S4.High() && S2.High() )||( S1.High() && S3.High() ))
      break;
  }*/
  int flag_turret = 0, turret_reached =0;
  while(1){  
    LineFollow34_Hybrid();
    Serial_Print();
    Move_Servo();
/*    if(flag_turret == 0){
      if(!Move_Turret_EncoderF()){
        turret_motor.Control(Check_Mirror(FWD,BCK),255);
      }else{
        turret_motor.Control(Check_Mirror(BCK,FWD),255);
        flag_turret = 1;
      }
    }
    if(flag_turret == 1){
      if(Move_TurretF())
        turret_reached = 1;
    }
*/
//    Move_TurretF();
    /*if(flag_turret == 0 && turret_sensor.Low()){
      turret_motor.Control(Check_Mirror(FWD,BCK),255);      
    }else{
      Move_TurretF();
      flag_turret = 1;
    }*/
/*    if(!turret_reached){
      if(Move_TurretF()){
        turret_reached = 1;
        turret_motor.Control(Check_Mirror(FWD,BCK),160);        
      }
    }else if(flag_turret == 0 && turret_sensor.High()){
      flag_turret++;
    }else if(flag_turret == 1 && turret_sensor.Low()){
      if(mirror){
        flag_turret++;
      }else{
        flag_turret = 4;
        turret_motor.Brake(0);
      }
    }else if(flag_turret == 2 && turret_sensor.High()){
      flag_turret++;
    }else if(flag_turret == 3 && turret_sensor.Low()){
      flag_turret++;
      turret_motor.Brake(0);
    }*/
    if(( S4.High()&& S2.High() )||( S1.High()&& S3.High() ))
      break;
  }  
/*  while(!( S4.High() && S2.High() )||( S1.High() && S3.High() )){
    LineFollow34_Slow();
    Move_Servo();
    Move_TurretF();
    /*if(flag_turret == 1 || turret_sensor.Low()) {
      turret_motor.Brake(255);
      flag_turret = 1;
    }
    if(( S4.High() && S2.High() )||( S1.High() && S3.High() ))
      break;
  }*/
  Motors_Brake(255,255);  
//  while(!Move_Turret_EncoderF()){
//    turret_motor.Control(Check_Mirror(FWD,BCK),255);
//  }
  if(mirror){
    turret_motor.Control(Check_Mirror(BCK,FWD),255);
    while(!Move_TurretF());
//    encoder_turret = 0;
//    encoder_turret_target = 250;
//    turret_motor.Control(Check_Mirror(FWD,BCK),255);
//    while(!Move_Turret_EncoderF());
    delay(200);
  }else{
    turret_motor.Control(Check_Mirror(BCK,FWD),255);
    while(!Move_TurretF());
    encoder_turret = 0;
    encoder_turret_target = 150;
    turret_motor.Control(Check_Mirror(FWD,BCK),255);
    while(!Move_Turret_EncoderF());
    delay(200);
  }
  /*
  if(flag_turret == 0){
    Motors_Brake(255,255);
    if(!Move_Turret_EncoderF()){
      turret_motor.Control(Check_Mirror(FWD,BCK),255);
      while(!Move_Turret_EncoderF());
      turret_motor.Control(Check_Mirror(BCK,FWD),255);
      flag_turret = 1;
      while(!Move_TurretF()){
        LAPTOP.println("CT1");
      }
    }else{
      turret_motor.Control(Check_Mirror(BCK,FWD),255);
      flag_turret = 1;
      while(!Move_TurretF()){
      LAPTOP.println("CT2");
      }
    }
  }else{
    Motors_Brake(255,255);
    while(!Move_TurretF()&&turret_sensor.Low()){
      LAPTOP.println("CT3");
    }
  }*/
}

void Drop_Two_Leaves(){
  LAPTOP.println("Dropping first two leaves");
  Motors_Brake(255,255);
  //while(turret_sensor.High());
  //turret_motor.Brake(255);
/*  int temp = millis() - prevmillis;
*/
//  delay(600);
//  Toggle_Wait();
  if(mirror){
    delay(1500);
    Actuate_High(RIGHT_VG);
    delay(750);

    encoder_turret = 0;
    encoder_turret_target = 100+turret_ang_change*50;
  
    turret_motor.Control(Check_Mirror(FWD,BCK),255);
    while(!Move_Turret_EncoderF());

    delay(1500);
    Actuate_High(LEFT_VG);
    delay(750);
  }else{
    delay(1500);
    Actuate_High(LEFT_VG);
    delay(750);
  
    encoder_turret = 0;
    encoder_turret_target = 175+turret_ang_change*50;
  
    turret_motor.Control(Check_Mirror(FWD,BCK),255);
    while(!Move_Turret_EncoderF());
  
    delay(1500);
  
    Actuate_High(RIGHT_VG);
    delay(750);
  //Toggle_Wait();
  //delay(1000);
  }
}

void Latitude_To_Last_Leaf(){
  Move_Forward(20,20);
  delay(500);
  Motors_Brake(255,255);
  delay(200);
  Parameters_Reset();  //TURN TO LATITUDE
  motor1.Brake(255);
  motor2.Control(FWD,75);
  delay(300); 
  servo1.Middle();
  servo2.Middle();
  encoder_turret = 0;
  encoder_turret_target = Check_Mirror(TURRET_ANG3MF, TURRET_ANG3F);
  turret_motor.Control(Check_Mirror(BCK,FWD),255);
  turret_reached = 0;
  while(S2.Low()&&S3.Low()){
    Serial_Print();
    if(!turret_reached){
      if(Move_TurretF())
        turret_reached = 1;
    }
  }
  turret_motor.Brake(0);
  Motors_Brake(255,255);
  delay(200);
  if(!turret_reached)
    turret_motor.Control(Check_Mirror(BCK,FWD),255);
  if(S1.Low() && S2.Low() && S3.Low() && S4.Low()){
    motor1.Control(FWD, 20);
    motor2.Brake(255);
    while(S2.Low()){
      if(!turret_reached){
      if(Move_TurretF())
        turret_reached = 1;
      }
    }
  }
  LAPTOP.println("Aligned to straight line");
  LAPTOP.println("Line following to next junction");
  while(1){
    LineFollow_Straight_Precision_slow(); //for latitude
    Serial_Print();
    if(!turret_reached){
      if(Move_TurretF())
        turret_reached = 1;
    }
    if(( S4.High() && S3.High() && S2.High() )||( S1.High() && S2.High() && S3.High()  ))
      break;
  }
  turret_motor.Brake(0);
  Motors_Brake(255,255);
  delay(500);
  Move_Forward(20,20);
  delay(600);
  Motors_Brake(255,255);
  delay(200);
  //  while(!Move_TurretF());
//  Toggle_Wait();
  if(!turret_reached)
    turret_motor.Control(Check_Mirror(BCK,FWD),255);
  motor1.Control(FWD,0);
  Turn_and_Align1(1,65); 
  turret_motor.Brake(0);
  Motors_Brake(255,255);
  delay(100);
//  Toggle_Wait(); // ADDED!! 
/*  int flag_turret = 0;
  turret_motor.Control(Check_Mirror(BCK,FWD),255);
  while(S2.Low() && S3.Low() && S1.Low() && S4.Low()){
    Serial_Print();
    if(!flag_turret){
      if(Move_TurretF())
        flag_turret = 1;
    }else if(flag_turret == 1){
      prevmillis = millis();
      if(!sharp_detected1){
        if(Read_Sharp()>120&&Read_Sharp()<320){
          if(millis()-prevmillis>1)
            sharp_detected1 = true;
        }else{
          prevmillis = millis();
        }
      }
      if(!sharp_detected2&&sharp_detected1){
        if(Read_Sharp()>320){
          if(millis()-prevmillis>5)
            sharp_detected2 = true;
        }else{
          prevmillis = millis();
        }
      }
/*      if(sharp_detected1&&sharp_detected2){
        encoder_turret = 0;
        encoder_turret_target = Check_Mirror(450,450);
        turret_motor.Control(Check_Mirror(FWD,BCK),255);
        flag_turret = 2;
      }
    }else{
      if(Read_Sharp()<200){
        Move_Turret_EncoderF();
        sharp_detected2 = 2;
        break;
      }
    }
    /*    if(encoder_turret_flag == 0 && Move_TurretF() == 1) {
      encoder_turret = 0; // reset turret
      encoder_turret_flag = 1;
      encoder_turret_target = 200;
    }
    if(encoder_turret_flag == 1 && encoder_turret>encoder_turret_target){      
      turret_motor.Brake(255);  
    }
//    Move_TurretF();
    if(line_detected)
      motor2.Control(FWD,20);
    else
      motor1.Control(FWD,20);
  }*/
//  turret_motor.Control(Check_Mirror(BCK,FWD),255);
  while(S2.Low() && S3.Low() && S1.Low() && S4.Low()){
    Serial_Print();
    if(!turret_reached){
      if(Move_TurretF())
        turret_reached = 1;
    }
    if(line_detected)
      motor2.Control(FWD,20);
    else
      motor1.Control(FWD,20);
  }
  LAPTOP.println("Moving to third leaf");
}

void To_Last_Leaf(){
  Parameters_Reset();
/*  if(!sharp_detected1){
    prevmillis = millis();
    while(1){
      Query_Launchpad();
      LineFollow12_Slow();
      Move_TurretF();
      if(Read_Sharp()>120&&Read_Sharp()<320){
        if(millis()-prevmillis>1)
          break;
      }else{
        prevmillis = millis();
      }
    }
  }
  if(!sharp_detected2){
    prevmillis = millis();
    while(1){
      Query_Launchpad();
      LineFollow12_Slow();
      Move_TurretF();
      if(Read_Sharp()>320){
        if(millis()-prevmillis>5)
          break;
      }else{
        prevmillis = millis();
      }
    }
  }*/
  Motors_Brake(255,255);
  delay(200);
//  Toggle_Wait();
//  if(sharp_detected2 != 2){
//  if(sharp_detected1&&sharp_detected2){
  while(Read_Sharp()<350){
    LineFollow12_Slow();
    if(!turret_reached){
      if(Move_TurretF())
        turret_reached = 1;
    }
  }
  encoder_turret = 0;
  encoder_turret_target = Check_Mirror(500,525);
  turret_motor.Control(Check_Mirror(FWD,BCK),255);
//  }
/*  if(mirror){
    while(analogRead(SHARP_SENSOR_PIN)>threshold_mid){
      Query_Launchpad();
      LineFollow12_Slow();
      Move_Turret_EncoderF();
    }
  }else{
    while(analogRead(SHARP_SENSOR_PIN)<threshold_mid){
      Query_Launchpad();
      LineFollow12_Slow();
      Move_Turret_EncoderF();
    }    
  }
  while(analogRead(SHARP_SENSOR_PIN)>threshold_edge){
    Query_Launchpad();
    LineFollow12_Slow();
    Move_Turret_EncoderF();  
  }*/
  Parameters_Reset();
  /*while(!LineFollow12_Encoders(1500,1)){
    Move_Turret_EncoderF();
  }*/
  turret_reached = 0;
  while(Read_Sharp()>100){
    LineFollow12_Slow();
    if(!turret_reached){
      if(Move_Turret_EncoderF())
        turret_reached = 1;
    }
  }
//  }
  Motors_Brake(255,255);
  if(!turret_reached)
  while(!Move_Turret_EncoderF());
  Parallelogram_Down()
  delay((omit_leaf1&&omit_leaf2)*250);
  Parallelogram_Stop();
//  Toggle_Wait();
  delay(300);//-(omit_leaf1&&omit_leaf2)*250);
}

void Drop_Last_Leaf(){
  LAPTOP.println("Dropping Third Leaf");
  Actuate_High(MIDDLE_VG);
  delay(200); //latest change on 23rd feb delay was there before 400 
}

void To_First_Bud(){
  servo1.Middle();
  servo2.Middle();
  if(omit_leaf1&&omit_leaf2&&omit_leaf3){
    Parameters_Reset();
    while(!LineFollow12_Encoders(3000,1));
    Motors_Brake(255,255);
  }
  //Linefollowing centred on S2 and S1. Linefollowing with brake till bud junction
  
  encoder_turret_target = Check_Mirror(TURRET_ANG4MF, TURRET_ANG4F);
  turret_motor.Control(Check_Mirror(FWD,BCK),255);
  int flag_turret = 0, turret_reached =0, locomotion_reached = 0;
  while(!LineFollow12_molu()){
//    if(!locomotion_reached)
//      locomotion_reached = LineFollow12_molu();

/*    if(flag_turret == 0){
      if(!Move_Turret_EncoderF()){
        turret_motor.Control(Check_Mirror(FWD,BCK),135);
      }else{
        turret_motor.Control(Check_Mirror(BCK,FWD),100);
        flag_turret = 1;
      }
    }
    if(flag_turret)
      if(Move_TurretF()&&locomotion_reached)
        break;
*/
      if(flag_turret == 0){
        if(Move_TurretF1()){
//          Motors_Brake(255,255);
//          while(1);
          flag_turret = 1;
          encoder_turret_target += 250;
          turret_motor.Control(Check_Mirror(FWD,BCK),255);  
        }
      }
      if(flag_turret)
        Move_Turret_EncoderF();
/*    if(!turret_reached){
      if(Move_TurretF()){
        turret_reached = 1;
        turret_motor.Control(Check_Mirror(FWD,BCK),160);        
      }
    }else if(flag_turret == 0 && turret_sensor.High()){
      flag_turret++;
    }else if(flag_turret == 1 && turret_sensor.Low()){
      flag_turret++;
    }else if(flag_turret == 2 && turret_sensor.High()){
      flag_turret++;
    }else if(flag_turret == 3 && turret_sensor.Low()){
      flag_turret++;
      turret_motor.Brake(0);
    }*/
    LAPTOP.println("Final LF to Bud");
    Serial_Print();
//    if(locomotion_reached && flag_turret == 4)
//      break;
  }
  Motors_Brake(255,255);
  if(flag_turret == 0){
    while(!Move_TurretF1());
    flag_turret = 1;
    encoder_turret_target += 250;
    turret_motor.Control(Check_Mirror(FWD,BCK),255);  
  }
  if(flag_turret)
    while(!Move_Turret_EncoderF());
  
  /*
  int flag_turret = 1;
  while(1){
    if(flag_turret == 0 && turret_sensor.Low())
      flag_turret += 1;
    else if(flag_turret == 1 && turret_sensor.High())
      flag_turret += 1;
    else if(flag_turret == 2 && turret_sensor.Low())
      flag_turret += 1;
    else if(flag_turret == 3 && turret_sensor.High())
      flag_turret += 1;
    else if(flag_turret == 4 && turret_sensor.Low()) {
      flag_turret += 1;
      if(!mirror)
        turret_motor.Brake(255);
    } else if(flag_turret == 5 && turret_sensor.High() && mirror)
      flag_turret += 1;
    else if(flag_turret == 6 && turret_sensor.Low() && mirror) {
      flag_turret += 1;
      turret_motor.Brake(255);
    }
      
    if(LineFollow12())
      break;    
  }
  while(!LineFollow12_Slow()) {
    if(flag_turret == 0 && turret_sensor.Low())
      flag_turret += 1;
    else if(flag_turret == 1 && turret_sensor.High())
      flag_turret += 1;
    else if(flag_turret == 2 && turret_sensor.Low())
      flag_turret += 1;
    else if(flag_turret == 3 && turret_sensor.High())
      flag_turret += 1;
    else if(flag_turret == 4 && turret_sensor.Low()) {
      flag_turret += 1;
      if(!mirror)
        turret_motor.Brake(255);
    } else if(flag_turret == 5 && turret_sensor.High() && mirror)
      flag_turret += 1;
    else if(flag_turret == 6 && turret_sensor.Low() && mirror) {
      flag_turret += 1;
      turret_motor.Brake(255);
    }
  }
  while ((flag_turret <=4 && !mirror) || (flag_turret<=6 && mirror))
{
    
    if(flag_turret == 0 && turret_sensor.Low())
      flag_turret += 1;
    else if(flag_turret == 1 && turret_sensor.High())
      flag_turret += 1;
    else if(flag_turret == 2 && turret_sensor.Low())
      flag_turret += 1;
    else if(flag_turret == 3 && turret_sensor.High())
      flag_turret += 1;
    else if(flag_turret == 4 && turret_sensor.Low()) {
      flag_turret += 1;
      if(!mirror)
        turret_motor.Brake(255);
    } else if(flag_turret == 5 && turret_sensor.High() && mirror)
      flag_turret += 1;
    else if(flag_turret == 6 && turret_sensor.Low() && mirror) {
      flag_turret += 1;
      turret_motor.Brake(255);
      
    }
      
}*/

  //Pick up, reverse and Go to Tokyo

 while(parallelogram_sensor.Low()){
  Parallelogram_Up(); 
  LAPTOP.println("Waiting for parallelogram till sensor high");
 }
 Parallelogram_Stop();

  // Toggle_Wait(); //Removed before first match;
  delay(1000);
  Actuate_Low(GRIPPER);                               // to pick up bud 1
  ///Take Parallelogram to lowest position
  
  Parallelogram_Down(); 
  delay(650+(omit_leaf1&&omit_leaf2)*250);
  Parallelogram_Stop();
  delay(200);
  

  LAPTOP.println("Reverse");
}

void Auto_Reset(){
  Motors_Brake(255,255);
//  turret_motor.Brake(0);
//  Toggle_Wait();  
  servo1.Home();
  servo2.Home();
//  turret_motor.Control(Check_Mirror(FWD,BCK),255);
//  delay(500);
  turret_motor.Brake(0);
  Move_Turret_Dir('c');  
//  Move_Turret_Dir('c');
  Parallelogram_Reset();
  ps_complete = true;
}

void Tokyo(){
  ///Take Parallelogram to mid position before reverse
  
  Parallelogram_Up();                                        //to raise para after black tape, to avoid count while shaking
  while(!parallelogram_sensor.High());
  delay(1200);
  Parallelogram_Stop();
  Parameters_Reset();
  Move_Back(Check_Mirror(25,25),Check_Mirror(25,25));
//  Run_For_Encoder_Count(500);
//  Move_Back(Check_Mirror(80,80),Check_Mirror(140,140));
//  Run_For_Encoder_Count(700);
//  Move_Back(Check_Mirror(80,80),Check_Mirror(140,140));
//  Move_Back(200,255);
  Run_For_Encoder_Count(Check_Mirror(7550,6300));
  Motors_Brake(255,0);
  Parameters_Reset();
  motor2.Control(BCK,0);
  Turn_and_Align1(4,75);
  Motors_Brake(255,255);
  delay(300);
//  Toggle_Wait();
//  if(S1.Low() && S2.Low() && S3.Low() && S4.Low() ){
    if(line_detected){
      while(S2.Low() && S3.Low()){
        motor2.Control(FWD,25);
      }
    }else{
      while(S3.Low() && S2.Low()){
        motor1.Control(FWD,20);
      }
    }
//  }
//  Motors_Brake(255,255);
//  delay(100);
//  Toggle_Wait();
  LAPTOP.println("This is Tokyo");
/*  while(encoder_motor2<9000){ 
    Query_Launchpad();
    if((S2.High() || S3.High() )&&encoder_motor2>2000)
      break;
  }*/  
//  Motors_Brake(255,255);
  LAPTOP.println("Left Turn Done");
//  delay(100);

//  motor2.Control(FWD,30); //To get back on track
//  while(S2.Low());
//  while(S3.Low());
//  Motors_Brake(255,255);
//  delay(100);
  ///Take Parallelogram to top position  
//  parallelogram_count = 0;
  Parallelogram_Up();

  //Line Follow to Manual Bot
  Parameters_Reset();  
  LAPTOP.println("Ready to linefollow");  
  /*while(LineFollow_Encoders(500,3)){
    Parallelogram_Tripped();
  }*/
  
  while(1){
    LAPTOP.print("Linefollow ONE");
    LineFollow_Straight_Fast();
    Parallelogram_Tripped();
    if((S3.High() && S1.High()) || (S4.High() && S2.High()) || (S3.High() && S2.High()))
      break;
  }
//  Move_Forward(40,40);  //To bypass junction
//  delay(200);
}
    
void To_Bud_Transfer(){

  /*
  // Old LineFollow
  while(1){
    LAPTOP.print("Linefollow Precision");
    LineFollow_Straight_Precision();
    Parallelogram_Reached(1);
    if(( S3.High() && S1.High() )||( S4.High() && S2.High())||(S3.High() && S2.High() ))
      break;
  }*/

  Parameters_Reset();
  while(1){
    Parallelogram_Tripped();
    
    if(bud_count==0){
      LAPTOP.println("BUD ONE");
      if(LineFollow_Encoders(6450,4))
        break;
//      if(Received_TSOP()){
//        break;        
//      }
    }else{
      LAPTOP.println("BUD TWO OR THREE");
      if(LineFollow_Straight_Precision())
        break;
      if(Received_TSOP()){
/*        if(S4.Low())
          motor2.Control(FWD,25);
        while(S4.Low());
        Motors_Brake(255,255);*/
        break;
      }
    }
  }
  distance_to_curve2 = (encoder_motor1+encoder_motor2)/2;
  Motors_Brake(255,255);
//  while(!Parallelogram_Tripped());
  Parallelogram_Up();
  delay(75);
  Parallelogram_Stop();
  delay(150);
/*  if(bud_count == 0){
    if(S4.Low())
      motor2.Control(FWD,25);
    while(S4.Low());
    Motors_Brake(255,255);
  }*/
  LAPTOP.println("Meet the Manual Bot");
}

int Received_TSOP(){

  if( digitalRead(COMM_TSOP_M1) == LOW || digitalRead(COMM_TSOP_M2) == LOW ) { // if Comm is on
    return 1;
  }
  else
    return 0;
  
}

void Wait_For_TSOP(){
  // Communication Code
//    Parallelogram_Stop();
  long int temp_millis = millis(), wait_time = 5, wait_millis = millis();
    while(1){
      if( millis() - wait_millis > 20000)
        return;
      if( digitalRead(COMM_TSOP_M1) == LOW || digitalRead(COMM_TSOP_M2) == LOW || digitalRead(COMM_TSOP_1) == LOW || digitalRead(COMM_TSOP_2) == LOW) { // if Comm is on
        if( millis() - temp_millis > wait_time) { // AND for a long time
          break;
        }
      }
      else
        temp_millis = millis();
    }
}
void Wait_For_TSOP1(){
  // Communication Code
//    Parallelogram_Stop();
  long int temp_millis = millis(), wait_time = 5, wait_millis = millis();
  if(mirror) {
    while(1){
      if( millis() - wait_millis > 10000)
        return;
      if( digitalRead(COMM_TSOP_M1) == LOW || digitalRead(COMM_TSOP_M2) == LOW ) { // if Comm is on
        if( millis() - temp_millis > wait_time) { // AND for a long time
          break;
        }
      }
      else
        temp_millis = millis();
    }
  } else {
    while(1){
      if( millis() - wait_millis > 10000)
        return;
      if( digitalRead(COMM_TSOP_1) == LOW || digitalRead(COMM_TSOP_2) == LOW ) { // if Comm is on
        if( millis() - temp_millis > wait_time) { // AND for a long time
          break;
        }
      }
      else
        temp_millis = millis();
    }
  }
}


void Transfer_Bud(){
 
  Wait_For_TSOP();
//  while( !(digitalRead(COMM_TSOP_1) == LOW || digitalRead(COMM_TSOP_2) == LOW) );
 // Toggle_Wait();
  //delay(1000);
  Actuate_High(GRIPPER);
  LAPTOP.println("Ready to go for two and three");
  delay(1000);
  bud_count++;
 // if( bud_count == 2)
//    path_phase -= 5;
}

void To_Curve2(){
  LAPTOP.println("Goint to Curve2"); 
  Parameters_Reset();                  
  Move_Back(90,150);
  if(bud_count == 0){
    Run_For_Encoder_Count(2100);// to check
  }else{
    Run_For_Encoder_Count(2500);// to check
  }
  Motors_Brake(0,255);
  motor1.Control(BCK,0);
  Turn_and_Align(1,Check_Mirror(80,115));
  Motors_Brake(255,255);
  delay(200);
//  Toggle_Wait();
  Parallelogram_Down();
  local_flag = 0;
  if(S1.Low()&&S2.Low()&&S3.Low()&&S4.Low()){
    if(line_detected){
      while(S2.Low()){ 
        motor1.Control(FWD,25);
        if(Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_BOTTOM)){
          Parallelogram_Stop();   
          Parallelogram_Up();
          local_flag = 1;
          prevmillis = millis();
        }
        if( parallelogram_sensor.High() && local_flag)
          Parallelogram_Stop();
        if( parallelogram_sensor.Low() && local_flag)
          Parallelogram_Stop();
      }
    }else{
      while(S3.Low()){
        motor2.Control(FWD,20);
        if(Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_BOTTOM)){
          Parallelogram_Stop();   
          Parallelogram_Up();
          local_flag = 1;
          prevmillis = millis();
        }
        if( parallelogram_sensor.High() && local_flag)
          Parallelogram_Stop();
        if( parallelogram_sensor.Low() && local_flag)
          Parallelogram_Stop();
      }
    }
  }
  Motors_Brake(255,255);
  delay(50);
//  Toggle_Wait();
//  Parallelogram_Down();
  /*delay(600);
  while(S1.Low());
  while(S2.Low());
  //while(S3.Low());
  
  if (!mirror)
  {
  while(S3.Low());
    while(S4.Low());
  }
//  while(S4.Low()&&S3.Low()&&S1.Low()&&S2.Low());
  */
//  Motors_Brake(255,255);
//  delay(200);

}

void To_Next_Bud(){

  //int local_flag = 0;
  servo1.Middle();
  servo2.Middle();
  Motors_Brake(255,255);
  int delay_for_plgm = 150;
  if(bud_count == 1) 
    delay_for_plgm = 100;
  else
    delay_for_plgm = 150;
//  Parallelogram_Down();
  /*
  delay(500);
  Parallelogram_Stop();
  Toggle_Wait();
  parallelogram_count = 0;
  Parallelogram_Down();
  */
  if(bud_count==1||omit_leaf3){
    Parameters_Reset();
    while(!LineFollow_Encoders(1000,2)){
    if(Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_BOTTOM)){
      Parallelogram_Stop();   
      Parallelogram_Up();
      local_flag = 1;
      prevmillis = 0;
    }
    if( parallelogram_sensor.High() && local_flag){
      if(!prevmillis){
        prevmillis = millis();
        Parallelogram_Up();
      }else if(abs(millis() - prevmillis)>delay_for_plgm)
        Parallelogram_Stop();
    }
    if( parallelogram_sensor.Low() && local_flag)
      Parallelogram_Up();
    }
    Parallelogram_Down()
  }

  Parameters_Reset();
  prevmillis = millis();
  while(1){
    if(Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_BOTTOM)){
      Parallelogram_Stop();   
      Parallelogram_Up();
      local_flag = 1;
      prevmillis = 0;
    }
    if( parallelogram_sensor.High() && local_flag){
      if(!prevmillis){
        prevmillis = millis();
        Parallelogram_Up();
      }else if(abs(millis() - prevmillis)>delay_for_plgm)
        Parallelogram_Stop();
    }
    if( parallelogram_sensor.Low() && local_flag)
      Parallelogram_Up();
    if(LineFollow_Encoders(9000,5))
      break;
  }


  while(!LineFollow_Curve_Precision()){
    if(Trip_Switch(PARALLELOGRAM_TRIP_SWITCH_BOTTOM) && !local_flag){
      Parallelogram_Stop();   
      Parallelogram_Up();
      prevmillis = 0;
      local_flag = 1;
    }
    if( parallelogram_sensor.High() && local_flag){
      if(!prevmillis){
        prevmillis = millis();
        Parallelogram_Up();
      }else if(abs(millis() - prevmillis)>delay_for_plgm)
        Parallelogram_Stop();
    }
    if( parallelogram_sensor.Low() && local_flag)
      Parallelogram_Up(); 
//    if( parallelogram_sensor.Low())
//      Parallelogram_Stop();
  }
  Motors_Brake(255,255);
//  Parallelogram_Up();
//  while(!parallelogram_sensor.High());
//  delay(150);
  Parallelogram_Stop();
  
  if(bud_count == 2){
    Parameters_Reset();
    while(1){
      if(LineFollow_Encoders(3000,1))  
        break;
    }
  }
  Motors_Brake(255,255);
  delay(200);
/*  if(mirror){
    motor1.Control(FWD,20);
    while(S1.Low()||S2.Low());
    Motors_Brake(255,255);
  }*/
  LAPTOP.println("Reached next bud");

  //Toggle_Wait(); //Removed before first match
  
  Actuate_Low(GRIPPER);
  
  //Move_Parallelogram(BCK,1);
  Parallelogram_Down(); 
  delay(950);
  Parallelogram_Stop();
  //delay(800);
}


void Tokyo2(){

//  Toggle_Wait();
  LAPTOP.println("Going to reverse");
  Parameters_Reset();

  Parallelogram_Up();
  while(!parallelogram_sensor.High());  //to take the parallelogram up till the tape
  delay(850);
  Parallelogram_Stop();
  Move_Back(25,35);
  Run_For_Encoder_Count(1000);
  Parallelogram_Up();
  Move_Back(25,35);
  //Move_Back(Check_Mirror(80,120), Check_Mirror(250,160));
  if(bud_count == 2){
    Run_For_Encoder_Count(Check_Mirror(11500,11900));
  }else{
    Run_For_Encoder_Count(Check_Mirror(9500,8500));
  }
  Motors_Brake(255,0);
  Parameters_Reset();
  motor2.Control(BCK,30);
  Turn_and_Align(4,Check_Mirror(60,75));
//  motor2.Control(BCK,30);
//  delay(300);
/*  while(S3.Low() && S2.Low()){
    Parallelogram_Tripped();
  }*/  
  Motors_Brake(255,255);
  delay(200);
//  Toggle_Wait();
  if(line_detected){
    while(S3.Low() && S2.Low()){
      motor2.Control(FWD,20);
      Parallelogram_Tripped();
    }
  }else{
    while(S3.Low() && S2.Low()){
      motor1.Control(FWD,20);
      Parallelogram_Tripped();
    }
  }
  Motors_Brake(255,255);
  delay(120);
//  Toggle_Wait();
/*
  //delay(400);
  int local_flag = 0;
  Parameters_Reset();
  while(encoder_motor2<9000){ 
    Query_Launchpad();
    LAPTOP.println(encoder_motor2);
    Parallelogram_Tripped();
    if(encoder_motor2>2000){//latest change on 23rd fwb earlier it was s3 or s4 now only s3
      if(S4.High())
        local_flag = 1;
      if(S3.High() && local_flag){ 
        LAPTOP.println("Line Detected");
        break;
     }
    }
  }  
  Motors_Brake(255,255);*/
 // while(!Parallelogram_Tripped);
  Parallelogram_Stop();
  Wait_For_TSOP1();
  Parallelogram_Up();

  LAPTOP.println("Right turn completed");


  motor2.Control(FWD,30);
  while(S3.Low()&&S4.Low()){
    Parallelogram_Tripped();
  }

  Motors_Brake(255,255);
  delay(150);
//  Toggle_Wait();
  //parallelogram_count = 0;
  //Parallelogram_Up();
}

void The_End(){
  Move_Back(100,255);
  delay(700);
  Motors_Brake(100,100);
  Move_Parallelogram(BCK,1);
  LAPTOP.println("Stage two completed");
  
  Toggle_Wait();
  Move_Turret_Dir('a');
  Move_Turret_Dir('a');
  Move_Turret_Dir('a');
  Move_Turret_Dir('a');
  Move_Turret_Dir('c');
  Parallelogram_Reset();
}

int Move_TurretF(){
  if(encoder_turret>encoder_turret_target){
   if(turret_sensor.Low()){
     turret_motor.Brake(255);
     return 1;
   }
  }
  return 0;
}

int Move_TurretF1(){
  if(encoder_turret>encoder_turret_target){
   if(turret_sensor.Low()){
     return 1;
   }
  }
  return 0;
}

int Move_Turret_EncoderF(){
  if(encoder_turret>encoder_turret_target){
    turret_motor.Brake(255);
    return 1;
  }
  return 0;
}

int Move_Turret_EncoderF1(){
  if(encoder_turret>encoder_turret_target){
//    turret_motor.Brake(255);
    return 1;
  }
  return 0;
}

