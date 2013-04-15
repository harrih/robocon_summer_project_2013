/*
  Last edited - 10 Apr 2013 - 15:50 AJK
*/
#include <epos2.h>

int t;
EPOS2 e(&Serial1, 7);
EPOS2 e2(&Serial2, 3);

void setup() {
  Serial.begin(115200);
  Serial.println("Begun");
}


void loop() {
  int32_t t;
/*  t = e.get_device_version();
  Serial.print("E - ver : ");
  Serial.println(t, HEX);

  t = e.get_device_type();
  Serial.print("E - type : ");
  Serial.println(t, HEX);

  t = e.get_supported_drive_modes();
  Serial.print("E - Supported drive modes : ");
  Serial.println(t, HEX);
  
  t = e.get_rs232_timeout();
  Serial.print("E - timeout : ");
  Serial.print(t);

  t = e.set_rs232_timeout(100);
//  Serial.println("E - timeout : SET");
  Serial.print(" --> ");

  t = e.get_rs232_timeout();
//  Serial.print("E - timeout 2 : ");
  Serial.println(t);
*/
  t = e.get_max_acc();
  Serial.print("E - max acc : ");
  Serial.print(t);

  t = e.set_max_acc(4000);
//  Serial.println("E - max acc : SET");
  Serial.print(" --> ");
  
  t = e.get_max_acc();
//  Serial.print("E - max acc 2 : ");
  Serial.println(t);

  t = e.get_max_vel();
  Serial.print("E - max vel : ");
  Serial.print(t);

  t = e.set_max_vel(4000);
//  Serial.println("E - max vel : SET");
  Serial.print(" --> ");
  
  t = e.get_max_vel();
//  Serial.print("E - max vel 2 : ");
  Serial.println(t);

/*  t = e.get_max_pos();
  Serial.print("E - max pos : ");
  Serial.print(t);

  t = e.set_max_pos(400000);
//  Serial.println("E - max pos : SET");
  Serial.print(" --> ");

  t = e.get_max_pos();
//  Serial.print("E - max pos : ");
  Serial.println(t);

  t = e.get_min_pos();
  Serial.print("E - min pos : ");
  Serial.print(t);

  t = e.set_min_pos(-400000);
//  Serial.println("E - min pos : SET");
  Serial.print(" --> ");
  
  t = e.get_min_pos();
//  Serial.print("E - min pos 2 : ");
  Serial.println(t);
*/
  t = e.get_max_following_error();
  Serial.print("E - max following error : ");
  Serial.print(t);

  t = e.set_max_following_error(40000);
//  Serial.println("E - max following error : SET");
  Serial.print(" --> ");
  
  t = e.get_max_following_error();
//  Serial.print("E - max following error 2 : ");
  Serial.println(t);

  int8_t t2 = e.get_operation_mode();
  Serial.print("E - operation mode : ");
  if(t2 == 1)  Serial.print("1 - profile position mode");
  else if(t2 == -1)  Serial.print("-1 - position");
  else if(t2 == 3)  Serial.print("3 - profile velocity");
  else if(t2 == -2)  Serial.print("-2 - velocity");
  else Serial.print(t2);
  
  t = e.set_operation_mode(VELOCITY_MODE);
//  Serial.println("E - operation mode : SET");
  Serial.print(" --> ");
  
  t2 = e.get_operation_mode();
//  Serial.print("E - operation mode 2 : ");
  if(t2 == 1)  Serial.println("1 - profile position mode");
  else if(t2 == -1)  Serial.println("-1 - position");
  else if(t2 == 3)  Serial.println("3 - profile velocity");
  else if(t2 == -2)  Serial.println("-2 - velocity");
  else Serial.println(t2);

  t = e.get_status_word();
  Serial.print("E - status word 1 : ");
  Serial.println(t, HEX);

  t = e.get_control_word();
  Serial.print("E - control word 1 : ");
  Serial.print(t, HEX);
  
  t = e.set_control_word(0x06);
//  Serial.println("E - control word : SET Shutdown");
  //Serial.println(t, HEX);
  Serial.print(" --> Shutdown --> ");
  
  t = e.get_control_word();
//  Serial.print("E - control word 2 : ");
  Serial.println(t, HEX);
    
  t = e.get_status_word();
  Serial.print("E - status word 2 : ");
  Serial.println(t, HEX);

//  delay(3000);

  t = e.set_control_word(0x0F);
  Serial.print("E - control word :  Switch on and Enable --> ");
  //Serial.println(t, HEX);
  
  t = e.get_control_word();
//  Serial.print("E - control word 3 : ");
  Serial.println(t, HEX);

  t = e.get_status_word();
  Serial.print("E - status word 3 : ");
  Serial.println(t, HEX);
  
// ---------------------------------------------
// FOR E2
// ---------------------------------------------
//  int32_t t;
/*  t = e.get_device_version();
  Serial.print("E - ver : ");
  Serial.println(t, HEX);

  t = e.get_device_type();
  Serial.print("E - type : ");
  Serial.println(t, HEX);

  t = e.get_supported_drive_modes();
  Serial.print("E - Supported drive modes : ");
  Serial.println(t, HEX);
  
  t = e.get_rs232_timeout();
  Serial.print("E - timeout : ");
  Serial.print(t);

  t = e.set_rs232_timeout(100);
//  Serial.println("E - timeout : SET");
  Serial.print(" --> ");

  t = e.get_rs232_timeout();
//  Serial.print("E - timeout 2 : ");
  Serial.println(t);
*/
  t = e2.get_max_acc();
  Serial.print("E2 - max acc : ");
  Serial.print(t);

  t = e2.set_max_acc(4000);
//  Serial.println("E2 - max acc : SET");
  Serial.print(" --> ");
  
  t = e2.get_max_acc();
//  Serial.print("E2 - max acc 2 : ");
  Serial.println(t);

  t = e2.get_max_vel();
  Serial.print("E2 - max vel : ");
  Serial.print(t);

  t = e2.set_max_vel(4000);
//  Serial.println("E2 - max vel : SET");
  Serial.print(" --> ");
  
  t = e2.get_max_vel();
//  Serial.print("E2 - max vel 2 : ");
  Serial.println(t);

/*  t = e2.get_max_pos();
  Serial.print("E2 - max pos : ");
  Serial.print(t);

  t = e2.set_max_pos(400000);
//  Serial.println("E2 - max pos : SET");
  Serial.print(" --> ");

  t = e2.get_max_pos();
//  Serial.print("E2 - max pos : ");
  Serial.println(t);

  t = e2.get_min_pos();
  Serial.print("E2 - min pos : ");
  Serial.print(t);

  t = e2.set_min_pos(-400000);
//  Serial.println("E2 - min pos : SET");
  Serial.print(" --> ");
  
  t = e2.get_min_pos();
//  Serial.print("E2 - min pos 2 : ");
  Serial.println(t);
*/
  t = e2.get_max_following_error();
  Serial.print("E2 - max following error : ");
  Serial.print(t);

  t = e2.set_max_following_error(40000);
//  Serial.println("E2 - max following error : SET");
  Serial.print(" --> ");
  
  t = e2.get_max_following_error();
//  Serial.print("E2 - max following error 2 : ");
  Serial.println(t);

  //int8_t 
  t2 = e2.get_operation_mode();
  Serial.print("E2 - operation mode : ");
  if(t2 == 1)  Serial.print("1 - profile position mode");
  else if(t2 == -1)  Serial.print("-1 - position");
  else if(t2 == 3)  Serial.print("3 - profile velocity");
  else if(t2 == -2)  Serial.print("-2 - velocity");
  else Serial.print(t2);
  
  t = e2.set_operation_mode(VELOCITY_MODE);
//  Serial.println("E - operation mode : SET");
  Serial.print(" --> ");
  
  t2 = e2.get_operation_mode();
//  Serial.print("E - operation mode 2 : ");
  if(t2 == 1)  Serial.println("1 - profile position mode");
  else if(t2 == -1)  Serial.println("-1 - position");
  else if(t2 == 3)  Serial.println("3 - profile velocity");
  else if(t2 == -2)  Serial.println("-2 - velocity");
  else Serial.println(t2);

  t = e2.get_status_word();
  Serial.print("E2 - status word 1 : ");
  Serial.println(t, HEX);

  t = e2.get_control_word();
  Serial.print("E2 - control word 1 : ");
  Serial.print(t, HEX);
  
  t = e2.set_control_word(0x06);
//  Serial.println("E - control word : SET Shutdown");
  //Serial.println(t, HEX);
  Serial.print(" --> Shutdown --> ");
  
  t = e2.get_control_word();
//  Serial.print("E - control word 2 : ");
  Serial.println(t, HEX);
    
  t = e2.get_status_word();
  Serial.print("E2 - status word 2 : ");
  Serial.println(t, HEX);

//  delay(3000);

  t = e2.set_control_word(0x0F);
  Serial.print("E2 - control word :  Switch on and Enable --> ");
  //Serial.println(t, HEX);
  
  t = e2.get_control_word();
//  Serial.print("E - control word 3 : ");
  Serial.println(t, HEX);

  t = e2.get_status_word();
  Serial.print("E2 - status word 3 : ");
  Serial.println(t, HEX);




/* // FOR POSITION MODE 
  t = e.get_position();
  Serial.print("E - pos : set=");
  Serial.print(t);
  t = e.get_demand_position();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_actual_position();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.set_position(30000);
  Serial.println("E - pos : SET");

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  int32_t t3 = t-1;
  while(t3 != t) {
    t = e.get_position();
    Serial.print("E - pos : set=");
    Serial.print(t);
    t3 = e.get_demand_position();
    Serial.print("    demand= ");
    Serial.print(t3);
    t = e.get_actual_position();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  
  t = e.get_status_word();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);
*/

  // FOR VELOCITY MODE
  t = e.get_velocity();
  Serial.print("E - vel : set=");
  Serial.print(t);
  t = e.get_demand_velocity();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_actual_velocity();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.set_velocity(21 * 140);
  Serial.println("E - vel : SET");

  t = e2.set_velocity(21 * 140);
  Serial.println("E2 - vel : SET");

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  int32_t t3 = t-1;
  while(t3 != t) {
    if( Serial.available() )
      break;
    t = e.get_velocity();
    Serial.print("E - vel : set=");
    Serial.print(t);
    t3 = e.get_demand_velocity();
    Serial.print("    demand= ");
    Serial.print(t3);
    t = e.get_actual_velocity();
    Serial.print("    actual= ");
    Serial.println(t);
    t = e2.get_velocity();
    Serial.print("E2 - vel : set=");
    Serial.print(t);
    t3 = e2.get_demand_velocity();
    Serial.print("    demand= ");
    Serial.print(t3);
    t = e2.get_actual_velocity();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  
  t = e.get_control_word();
  Serial.print("E - control word 1 : ");
  Serial.print(t, HEX);
  
  t = e.set_control_word(0x04);
//  Serial.println("E - control word : SET Shutdown");
  //Serial.println(t, HEX);
  Serial.print(" --> Shutdown --> ");
  
  t = e.get_control_word();
//  Serial.print("E - control word 2 : ");
  Serial.println(t, HEX);
  t = e.get_status_word();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);





  
  
  


  /*

   Serial.println("");  
   // SECOND EPOS
   
   t = e2.get_device_version();
   Serial.print("E2 - ver : ");
   Serial.println(t, HEX);
   
   t = e2.get_device_type();
   Serial.print("E2 - type : ");
   Serial.println(t, HEX);
   
   t = e2.get_rs232_timeout();
   Serial.print("E2 - timeout 1 : ");
   Serial.println(t, HEX);
   
   t = e2.set_rs232_timeout(100);
   Serial.println("E2 - timeout : SET");
   //  Serial.println(t, HEX);
   
   t = e2.get_rs232_timeout();
   Serial.print("E2 - timeout 2 : ");
   Serial.println(t, HEX);
   
   t = e2.get_max_acc();
   Serial.print("E2 - max acc 1 : ");
   Serial.println(t, HEX);
   
   t = e2.set_max_acc(4000);
   Serial.println("E2 - max acc : SET");
   //Serial.println(t, HEX);
   
   t = e2.get_max_acc();
   Serial.print("E2 - max acc 2 : ");
   Serial.println(t, HEX);
   
   
   
   */


  Serial.println("DONE");

  Serial.println();
  Serial.println();
  while(1);
}



