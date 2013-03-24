
/** Function to obtain left and right encoder values **/

int Query_Launchpad(){  
  // Frame spec:
  //	Left encoder values LSB
  //	Left encoder values MSB
  //	Right encoder values LSB
  //	Right encoder values MSB
  int static last_encoder_diff = 0;
  long int starttime;
  LAUNCHPAD.write('S');

  if(!Launchpad_Wait(last_encoder_diff))
    return 0;
  diff_encoder_left = LAUNCHPAD.read();
//  if(!Launchpad_Wait(last_encoder_diff))
 //   return 0;
 // diff_encoder_left = diff_encoder_left + (uint16_t)(LAUNCHPAD.read()<<8);
  
  if(!Launchpad_Wait(last_encoder_diff))
    return 0;
  diff_encoder_right = LAUNCHPAD.read();
 // if(!Launchpad_Wait(last_encoder_diff))
  //  return 0;
  //diff_encoder_right = diff_encoder_right + (uint16_t)(LAUNCHPAD.read()<<8);

  if( diff_encoder_left == 255 ){
    LAUNCHPAD.write('L');
    starttime = millis();
    while(!LAUNCHPAD.available()){
      if( millis()-starttime > 5 ){
        LAPTOP.println("Launchpad didn't respond to L! :( ");
        //Abort();
        diff_encoder_left += last_encoder_diff;
      }
    }
  }
  
  if( diff_encoder_right == 255 ){
    LAUNCHPAD.write('R');
    starttime = millis();
    while(!LAUNCHPAD.available()){
      if( millis()-starttime > 5 ){
        LAPTOP.println("Launchpad didn't respond to R! :( ");
        //Abort();
        diff_encoder_right += last_encoder_diff;
      }
    }
  }
  
  if(mirror){
    encoder_motor2 += diff_encoder_left;
    encoder_motor1 += diff_encoder_right;
  }else{   
    encoder_motor1 += diff_encoder_left;
    encoder_motor2 += diff_encoder_right;
  }
  
  last_encoder_diff = (diff_encoder_left + diff_encoder_right)/2;
  
//  LAPTOP.println(last_encoder_diff);
  
  if(diff_encoder_left == 0 || diff_encoder_right == 0)
    LAPTOP.println("\n ZERO \n");
}

void Launchpad_Reset(){
  Query_Launchpad();
}

int Launchpad_Wait(int default_encoder_value){
  long int starttime = millis();
  while(!LAUNCHPAD.available()){
    if( millis()-starttime > 5 ){
      LAPTOP.println("Launchpad didn't respond! :( ");
      //Abort();
      encoder_motor1 += default_encoder_value;
      encoder_motor2 += default_encoder_value;
      return 0;
    }
  }
  return 1;
}
