//Autobot Master Systems Check

void Auto_MSC(){
  char msc_option = 'a';
  LAPTOP.println("Master Systems Check \n 'q' to quit process");
  //default switch off suction
  Actuate_High(LEFT_VG); Actuate_High(MIDDLE_VG); Actuate_High(RIGHT_VG); Actuate_High(GRIPPER);
  
  while(msc_option!='q'){
    LAPTOP.println(" a. Motors free roll \n s. Servos through 3 angles \n d. Turret complete functioning \n f. Parallelogram complete functioning \n g. Four solenoids \n h. Four Autonics Line Sensors \n j. Both SHARP sensors");
    msc_option = Serial_Wait();
    switch(msc_option){
      case 'a': MSC_Motors(); break;
      case 's': MSC_Servos(); break; 
      case 'd': MSC_Turret(); break; 
      case 'f': MSC_Parallelogram(); break;          
      case 'g': MSC_Solenoids(); break; 
      case 'h': MSC_Line_Sensors(); break;
      case 'j': MSC_SHARP(); break;     
    }
  }
  LAPTOP.println("Out of MSC");
}

void MSC_Motors(){
  char local_flag = 'a';
  LAPTOP.println(" a. Forward \n s. Reverse \n d. Brake");
  
  while(local_flag!='q'){
    local_flag = Serial_Wait();
    switch(local_flag){
      case 'a': Move_Forward(minimum_pwm, minimum_pwm); break;
      case 's': Move_Back(minimum_pwm, minimum_pwm); break;
      case 'd': Motors_Brake(HARDBRAKE, HARDBRAKE); break;
    }
  }
}

void MSC_Servos(){
  char local_flag = 'a';
  LAPTOP.println("a. Home \n s. Middle \n d. Extend");

  while(local_flag!='q'){
    local_flag = Serial_Wait();
    switch(local_flag){
      case 'a': servo1.Home(); servo2.Home(); break;
      case 's': servo1.Middle(); servo2.Middle(); break;
      case 'd': servo1.Extend(); servo2.Extend(); break;
    }
  }
}

void MSC_Turret(){
  char local_flag = 'a';
  LAPTOP.println(" a. Clockwise \n s. Anticlockwise \n d. Stop \n f. Clockwise till next strip \n g. Anticlockwise till next strip \n h. Clockwise for 1700 Encoder counts \n i. Anticlockwise for 1950 Encoder counts");

  while(local_flag!='q'){
    local_flag = Serial_Wait();
    switch(local_flag){
      case 'a': turret_motor.Control(FWD,100); break;
      case 's': turret_motor.Control(BCK,100); break;
      case 'd': turret_motor.Brake(255); break;
      case 'f': Move_Turret_Dir('c'); break;
      case 'g': Move_Turret_Dir('a'); break;
      case 'h': turret_motor.Control(FWD,100);
                encoder_turret = 0;
                while(encoder_turret<1700);
                turret_motor.Brake(255); break;
      case 'i': turret_motor.Control(BCK,100);
                encoder_turret = 0;
                while(encoder_turret<1950);
                turret_motor.Brake(255); break;
    }
  }  
}

void MSC_Parallelogram(){
  char local_flag = 'a';
  LAPTOP.println(" a. Up \n s. Down \n d. Stop \n f. To Top most position \n g. Initial position \n h. Stop at Tape (from bottom) \n j. Stop at Tape (from top)");

  while(local_flag!='q'){
  local_flag = Serial_Wait();
  switch(local_flag){
      case 'a': Parallelogram_Up(); break;
      case 's': Parallelogram_Down(); break;
      case 'd': Parallelogram_Stop(); break;
      case 'f': while(!Parallelogram_Tripped()); break;
      case 'g':  
                Parallelogram_Down();
                while(!digitalRead(PARALLELOGRAM_TRIP_SWITCH_BOTTOM));
                Parallelogram_Stop();
                break;
      case 'h': 
                Parallelogram_Up();
                while(!parallelogram_sensor.High());
                Parallelogram_Stop();
                break;    
     case 'j': 
                Parallelogram_Down();
                while(!parallelogram_sensor.Low());
                Parallelogram_Stop();
                break;            
    }
  }  
}

void MSC_Solenoids(){
  char local_flag = 'a';
  LAPTOP.println(" a. Switch on Left \n s. Switch on Middle \n d. Switch on Right \n f. Switch on Gripper \n g. Switch off all \nh. High V Piston \n j. Low V Piston");

  while(local_flag!='q'){
  local_flag = Serial_Wait();
  switch(local_flag){
      case 'a': Actuate_Low(LEFT_VG); break;
      case 's': Actuate_Low(MIDDLE_VG); break;
      case 'd': Actuate_Low(RIGHT_VG); break;
      case 'f': Actuate_Low(GRIPPER); break;
      case 'g': Actuate_High(LEFT_VG); Actuate_High(MIDDLE_VG); Actuate_High(RIGHT_VG); Actuate_High(GRIPPER); break;
      case 'h': Actuate_High(V_PISTON); break;
      case 'j': Actuate_Low(V_PISTON); break;
    }
  }  
}
  
void MSC_Line_Sensors(){
  while(1){
    LAPTOP.print(" S1: "); LAPTOP.print(S1.High());
    LAPTOP.print(" S2: "); LAPTOP.print(S2.High());
    LAPTOP.print(" S3: "); LAPTOP.print(S3.High());
    LAPTOP.print(" S4: "); LAPTOP.println(S4.High()); 
    if(LAPTOP.available()){
      if(LAPTOP.read() == 'q'){
        break;
      }
    }
  }
}

void MSC_SHARP(){
  while(1){
    LAPTOP.print("Inner SHARP value: "); LAPTOP.print(SHARP_SENSOR_PIN);
    //LAPTOP.print(" Outer SHARP value: "); LAPTOP.println(SHARP_SENSOR_OUTER); 
    if(LAPTOP.available()){
       if(LAPTOP.read() == 'q'){
         break;
       }
    }
  }
}  
