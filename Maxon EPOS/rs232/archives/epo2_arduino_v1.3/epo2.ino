/*
  Last edited - 10 Apr 2013 - 23:10 AJK
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
  t = e.get_motor_type();
  Serial.print("E - motor_type : ");
  Serial.print(t);

  t = e.set_motor_type(DC_MOTOR);
//  Serial.println("E - max following error : SET");
  Serial.print(" --> ");
  
  t = e.get_motor_type();
//  Serial.print("E - max following error 2 : ");
  Serial.println(t);

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

  t = e.get_misc_config();
  Serial.print("E - misc config : ");
  Serial.print(t);

  t = e.set_misc_config(0x0100);
//  Serial.println("E - max following error : SET");
  Serial.print(" --> ");
  
  t = e.get_misc_config();
//  Serial.print("E - max following error 2 : ");
  Serial.println(t);
  
  int8_t t2 = e.get_operation_mode();
  Serial.print("E - operation mode : ");
  if(t2 == 1)  Serial.print("1 - profile position mode");
  else if(t2 == -1)  Serial.print("-1 - position");
  else if(t2 == 3)  Serial.print("3 - profile velocity");
  else if(t2 == -2)  Serial.print("-2 - velocity");
  else Serial.print(t2);
  
  t = e.set_operation_mode(INTERPOLATED_POSITION_MODE);
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

/* 
  // ------------------------------------------------------------
  // FOR POSITION MODE 
  t = e.get_position_setting();
  Serial.print("E - pos : set=");
  Serial.print(t);
  t = e.get_demand_position();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_actual_position();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.set_position_setting(30000);
  Serial.println("E - pos : SET");

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  int32_t t3 = t-1;
  while(t3 != t) {
    t = e.get_position_setting();
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

/*
  // ------------------------------------------------------------
  // FOR VELOCITY MODE
  t = e.get_velocity_setting();
  Serial.print("E - vel : set=");
  Serial.print(t);
  t = e.get_demand_velocity();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_actual_velocity();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.set_velocity_setting(21 * 140);
  Serial.println("E - vel : SET");

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  while(1) {
    if( Serial.available() )
      break;
    t = e.get_velocity_setting();
    Serial.print("E - vel : set=");
    Serial.print(t);
    t = e.get_demand_velocity();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_actual_velocity();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  
  t = e.get_status_word();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);
*/


/*  // ------------------------------------------------------------
  // FOR PROFILE POSITION  
  t = e.get_profile_target_position();
  Serial.print("E - profile position : set=");
  Serial.print(t);
  t = e.get_demand_position();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_actual_position();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.get_profile_velocity();
  Serial.print("E - profile velocity : ");
  Serial.print(t);
  t = e.set_profile_velocity(21 * 100);
  Serial.print(" --> ");
  t = e.get_profile_velocity();
  Serial.println(t);
  
  t = e.get_profile_acceleration();
  Serial.print("E - profile acceleration : ");
  Serial.print(t);
  t = e.set_profile_acceleration(21 * 100);
  Serial.print(" --> ");
  t = e.get_profile_acceleration();
  Serial.println(t);
  
  t = e.get_profile_deceleration();
  Serial.print("E - profile deceleration : ");
  Serial.print(t);
  t = e.set_profile_deceleration(21 * 100);
  Serial.print(" --> ");
  t = e.get_profile_deceleration();
  Serial.println(t);
  
  t = e.get_profile_quickstop_deceleration();
  Serial.print("E - profile quickstop deceleration : ");
  Serial.print(t);
  t = e.set_profile_quickstop_deceleration(21 * 200);
  Serial.print(" --> ");
  t = e.get_profile_quickstop_deceleration();
  Serial.println(t);

  t = e.get_profile_motion();
  Serial.print("E - profile motion type : ");
  Serial.print(t);
  t = e.set_profile_motion(SINUSOIDAL_PROFILE);
  Serial.print(" --> ");
  t = e.get_profile_motion();
  Serial.println(t);

  t = e.set_profile_target_position(70000);
  Serial.println("E - profile position : SET");
  
  t = e.get_control_word();
  Serial.print("E - control word : ");
  Serial.print(t, HEX);
  t = e.set_control_word(0x7f);
  Serial.print(" --> Start profile position (rel, imm) --> ");  
  t = e.get_control_word();
  Serial.println(t, HEX);

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  while(1) {
    if( Serial.available() )
      break;
    t = e.get_profile_target_position();
    Serial.print("E - profile position : set=");
    Serial.print(t);
    t = e.get_demand_position();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_actual_position();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  
  t = e.get_status_word();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);
*/

/*
  // ------------------------------------------------------------
  // FOR PROFILE VELOCITY 
  t = e.get_profile_target_velocity();
  Serial.print("E - profile velocity : set=");
  Serial.print(t);
  t = e.get_demand_velocity();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_actual_velocity();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.get_profile_acceleration();
  Serial.print("E - profile acceleration : ");
  Serial.print(t);
  t = e.set_profile_acceleration(21 * 100);
  Serial.print(" --> ");
  t = e.get_profile_acceleration();
  Serial.println(t);
  
  t = e.get_profile_deceleration();
  Serial.print("E - profile deceleration : ");
  Serial.print(t);
  t = e.set_profile_deceleration(21 * 100);
  Serial.print(" --> ");
  t = e.get_profile_deceleration();
  Serial.println(t);
  
  t = e.get_profile_quickstop_deceleration();
  Serial.print("E - profile quickstop deceleration : ");
  Serial.print(t);
  t = e.set_profile_quickstop_deceleration(21 * 200);
  Serial.print(" --> ");
  t = e.get_profile_quickstop_deceleration();
  Serial.println(t);

  t = e.get_profile_motion();
  Serial.print("E - profile motion type : ");
  Serial.print(t);
  t = e.set_profile_motion(SINUSOIDAL_PROFILE);
  Serial.print(" --> ");
  t = e.get_profile_motion();
  Serial.println(t);

  t = e.set_profile_target_velocity(21 * 100);
  Serial.println(" profile target velocity : SET");
  
  t = e.get_control_word();
  Serial.print("E - control word : ");
  Serial.print(t, HEX);
  t = e.set_control_word(0x0f);
  Serial.print(" --> Start profile velocity --> ");  
  t = e.get_control_word();
  Serial.println(t, HEX);

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  while(1) {
    if( Serial.available() )
      break;
    t = e.get_profile_target_velocity();
    Serial.print("E - profile target velocity : set=");
    Serial.print(t);
    t = e.get_demand_velocity();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_actual_velocity();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  
  t = e.get_status_word();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);
*/

  // ------------------------------------------------------------
  // FOR INTERPOLATED POSITION PROFILE 
  t = e.get_max_ipm_buffer();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_actual_ipm_buffer();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);
  
  t = e.get_ipm_buffer_status();
  Serial.print("E - IPM buffer status : ");
  Serial.print(t, HEX);
  t = e.set_ipm_buffer(0);
  Serial.print(" --> IPM Buffer disabled --> ");
  t = e.get_ipm_buffer_status();
  Serial.println(t, HEX);
  
  t = e.get_ipm_buffer_status();
  Serial.print("E - IPM buffer status : ");
  Serial.print(t, HEX);
  t = e.set_ipm_buffer(1);
  Serial.print(" --> IPM Buffer enabled --> ");
  t = e.get_ipm_buffer_status();
  Serial.println(t, HEX);
  
  t = e.get_max_ipm_buffer();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_actual_ipm_buffer();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);

  t = e.get_position_window();
  Serial.print("E - position window : ");
  Serial.print(t);
  t = e.set_position_window(0xffffffff);
  Serial.print(" --> ");
  t = e.get_position_window();
  Serial.println(t);
  
  t = e.get_position_window_time();
  Serial.print("E - position window time : ");
  Serial.print(t);
  t = e.set_position_window_time(0);
  Serial.print(" --> ");
  t = e.get_position_window_time();
  Serial.println(t);
  
  t = e.get_profile_velocity();
  Serial.print("E - profile velocity : ");
  Serial.print(t);
  t = e.set_profile_velocity(21 * 160);
  Serial.print(" --> ");
  t = e.get_profile_velocity();
  Serial.println(t);
  
  t = e.get_profile_acceleration();
  Serial.print("E - profile acceleration : ");
  Serial.print(t);
  t = e.set_profile_acceleration(21 * 160);
  Serial.print(" --> ");
  t = e.get_profile_acceleration();
  Serial.println(t);
  
  // set PVTs 
  t = e.add_to_ipm_buffer(2000, 21 * 60, 2000);
  Serial.print(" Set 1 ");  
  t = e.add_to_ipm_buffer(5000, 21 * 70, 2000);
  Serial.print(" 2 ");  
  t = e.add_to_ipm_buffer(9000, 21 * 100, 2000);
  Serial.print(" 3 ");  
  t = e.add_to_ipm_buffer(15000, 21 * 140, 2000);
  Serial.println(" 4 ");  
  
  t = e.get_max_ipm_buffer();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_actual_ipm_buffer();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);
  
  t = e.get_control_word();
  Serial.print("E - control word : ");
  Serial.print(t, HEX);
  t = e.set_control_word(0x1F);
  Serial.print(" --> Start IPM --> ");  
  t = e.get_control_word();
  Serial.println(t, HEX);

  t = e.get_status_word();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  while(1) {
    if( Serial.available() )
      break;
    Serial.print("E - position : ");
    t = e.get_demand_position();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_actual_position();
    Serial.print("    actual= ");
    Serial.println(t);
    Serial.print("E - velocity : ");
    t = e.get_demand_velocity();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_actual_velocity();
    Serial.print("    actual= ");
    Serial.println(t);
    t = e.get_ipm_buffer_status();
    Serial.print("E - IPM buffer status : ");
    Serial.println(t, HEX);
  }
  
  t = e.get_status_word();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);





  // HALT code
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





  Serial.println("DONE");

  Serial.println();
  Serial.println();
  while(1);
}



