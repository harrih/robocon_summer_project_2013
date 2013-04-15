/*
  Last edited - 10 Apr 2013 - 23:10 AJK
*/
#include <epos2.h>

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
  
/*  t = e.get_misc_config_bits();
  Serial.print("E - misc config : ");
  Serial.print(t);
  t = e.set_misc_config(0x0100);
  Serial.print(" --> ");
  t = e.get_misc_config_bits();
  Serial.println(t);
*/  
  t = e.set_operation_mode(INTERPOLATED_POSITION_MODE);
  if(t) {
    Serial.println("Failed to set to interpolated position mode");
    
  }
/*
  t = e.get_statusword_bits();
  Serial.print("E - status word 1 : ");
  Serial.println(t, BIN);

  t = e.get_controlword_bits();
  Serial.print("E - control word 1 : ");
  Serial.print(t, BIN);
  t = e.set_controlword(0x06);
  Serial.print(" --> Shutdown --> ");
  t = e.get_controlword_bits();
  Serial.println(t, BIN);
  
  t = e.get_statusword_bits();
  Serial.print("E - status word 2 : ");
  Serial.println(t, BIN);

//  delay(3000);

  t = e.set_controlword(0x0F);
  Serial.print("E - control word :  Switch on and Enable --> ");
  t = e.get_controlword_bits();
  Serial.println(t, BIN);

  t = e.get_statusword_bits();
  Serial.print("E - status word 3 : ");
  Serial.println(t, BIN);
*/
/*
  // ------------------------------------------------------------
  // FOR POSITION MODE 
  t = e.get_position_mode_setting_value();
  Serial.print("E - pos : set=");
  Serial.print(t);
  t = e.get_position_demand_value();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_position_actual_value();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.set_position_mode_setting_value(30000);
  Serial.println("E - pos : SET");

  t = e.get_statusword_bits();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  int32_t t3 = t-1;
  while(t3 != t) {
    t = e.get_position_mode_setting_value();
    Serial.print("E - pos : set=");
    Serial.print(t);
    t3 = e.get_position_demand_value();
    Serial.print("    demand= ");
    Serial.print(t3);
    t = e.get_position_actual_value();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  while(1);
  t = e.get_statusword_bits();
  Serial.print("E - status word 5 : ");
  Serial.println(t, HEX);
*/
/*
  // ------------------------------------------------------------
  // FOR VELOCITY MODE
  t = e.get_velocity_mode_setting_value();
  Serial.print("E - vel : set=");
  Serial.print(t);
  t = e.get_velocity_demand_value();
  Serial.print("    demand= ");
  Serial.print(t);
  t = e.get_velocity_actual_value();
  Serial.print("    actual= ");
  Serial.println(t);

  t = e.set_velocity_mode_setting_value(21 * 140);
  Serial.println("E - vel : SET");

  t = e.get_statusword_bits();
  Serial.print("E - status word 4 : ");
  Serial.println(t, HEX);

  while(1) {
    if( Serial.available() )
      break;
    t = e.get_velocity_mode_setting_value();
    Serial.print("E - vel : set=");
    Serial.print(t);
    t = e.get_velocity_demand_value();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_velocity_actual_value();
    Serial.print("    actual= ");
    Serial.println(t);
  }
  
  t = e.get_statusword_bits();
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
  
  t = e.get_statusword_bits();
  Serial.print("E - status word 1 : ");
  Serial.println(t, BIN);

  t = e.get_controlword_bits();
  Serial.print("E - control word 1 : ");
  Serial.print(t, BIN);
  t = e.set_controlword(0x06);
  Serial.print(" --> Shutdown --> ");
  t = e.get_controlword_bits();
  Serial.println(t, BIN);
  
  t = e.get_statusword_bits();
  Serial.print("E - status word 2 : ");
  Serial.println(t, BIN);

//  delay(3000);

  t = e.set_controlword(0x0F);
  Serial.print("E - control word :  Switch on and Enable --> ");
  t = e.get_controlword_bits();
  Serial.println(t, BIN);

  t = e.get_statusword_bits();
  Serial.print("E - status word 3 : ");
  Serial.println(t, BIN);
  
  t = e.get_ipm_maximum_buffer_size();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_ipm_actual_buffer_size();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);
  
  t = e.get_ipm_buffer_status_bits();
  Serial.print("E - IPM buffer status : ");
  Serial.print(t, BIN);
  t = e.set_ipm_buffer_clear(0);
  Serial.print(" --> IPM Buffer disabled --> ");
  t = e.get_ipm_buffer_status_bits();
  Serial.println(t, BIN);
  /*
  t = e.get_ipm_maximum_buffer_size();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_ipm_actual_buffer_size();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);
  */
  t = e.get_ipm_buffer_status_bits();
  Serial.print("E - IPM buffer status : ");
  Serial.print(t, BIN);
  t = e.set_ipm_buffer_clear(1);
  Serial.print(" --> IPM Buffer enabled --> ");
  t = e.get_ipm_buffer_status_bits();
  Serial.println(t, BIN);
  
  t = e.get_ipm_maximum_buffer_size();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_ipm_actual_buffer_size();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);


  t = e.get_statusword_bits();
  Serial.print("E - status word TEST : ");
  Serial.println(t, BIN);
 
  // set PVTs 
while(1) {
  Serial.println("Adding frames ... ");
/*  epos2_ipm_frame t_ipm[5];
  t_ipm[0].p = 100;  t_ipm[0].v = 30;  t_ipm[0].t = 1000;
  t_ipm[1].p = 150;  t_ipm[1].v = 40;  t_ipm[1].t = 2000;
  t_ipm[2].p = 200;  t_ipm[2].v = 50;  t_ipm[2].t = 3000;
  t_ipm[3].p = 250;  t_ipm[3].v = 60;  t_ipm[3].t = 4000;
  t_ipm[4].p = 300;  t_ipm[4].v = 70;  t_ipm[4].t = 5000;
  e.add_to_ipm_buffer(t_ipm, 5);
*/
  
  t = e.send(get_frame(0x13, 3, 0x20C1, 0x00, 0x10, 0)); // total number of bytes to write ... is it 16bit or 8bit ?
  if(t) { 
    Serial.print("Error initiating segment : ");	
    Serial.println(t);
    while(1);
  }
  Serial.println("sent");
  while(!Serial1.available());
  //Serial.println("recd");
  if(Serial1.read()==0) {
    //Serial.println("got 0");
    Serial1.write('O');
    //Serial.println("sent ok");
  
    while(!Serial1.available());
    //Serial.println("Got 2");
    while(Serial1.available()) {
      //Serial.println(Serial1.read(), HEX);
      Serial1.read();
    }
/*    int ls2[4];
    ls2[0] = 0x0001;
    ls2[1] = 0x0000;
    ls2[2] = 0x0000;
    ls2[3] = 0;
    uint16_t crc = e.get_crc(ls2, 4);
    Serial.print("got crc for checking : ");
    Serial.println(crc, HEX);    
*/  }
  Serial.println("Segment initiated");
  int ls[11];
  ls[0] = 0x1508; // opcode len-1(WORDS)

  ls[1] = 0x10 + (1<<7); // res toggle(0x0040) len-1(BYTES)
  
  ls[2] = 2000;//0x0000; // LSBpos
  ls[3] = 0x0000; // MSBpos
  ls[4] = 1260;//0x0000; // LSBvel
  ls[5] = 200<<8; // TIME MSBvel
  
  ls[6] = 0x0009; // LSBpos
  ls[7] = 0x0000; // MSBpos
  ls[8] = 0x0009; // LSBvel
  ls[9] = 0x1000; // TIME MSBvel
  
  ls[10] = 0;
  uint16_t crc = e.get_crc(ls, 11);
//  Serial.print("got crc : ");
//  Serial.println(crc, HEX);
  delay(800);
  while(Serial1.available())  Serial.print(Serial1.read());
  //Serial.println("Serial1 flushed");
  Serial1.write(0x15); 
  //Serial.println("waiting ...");
  while(!Serial1.available());
  //Serial.println("got something");
  char c = Serial1.read();
  Serial.print("Opcode result ! : ");
  Serial.print(c);
  Serial.print(" -- ");
  Serial.println(c, HEX);
  if(c != 'O')
    while(1);
    
  Serial1.write(ls[0]); // len rs232
  Serial1.write(ls[1]); // control byte
  Serial1.write(ls[1]>>8);
 
  Serial1.write(ls[2]); // data 0
  Serial1.write(ls[2]>>8);
  Serial1.write(ls[3]); // data 1
  Serial1.write(ls[3]>>8);
  Serial1.write(ls[4]); // data 2
  Serial1.write(ls[4]>>8);
  Serial1.write(ls[5]); // data 3
  Serial1.write(ls[5]>>8);
  
  Serial1.write(ls[6]); // data 0
  Serial1.write(ls[6]>>8);
  Serial1.write(ls[7]); // data 1
  Serial1.write(ls[7]>>8);
  Serial1.write(ls[8]); // data 2
  Serial1.write(ls[8]>>8);
  Serial1.write(ls[9]); // data 3
  Serial1.write(ls[9]>>8);
  /*
  Serial1.write(0x09); // data 0
  Serial1.write(0x00);
  Serial1.write(0x00); // data 1
  Serial1.write(0x00);
  Serial1.write(0x09); // data 2
  Serial1.write(0x00);
  Serial1.write(0x00); // data 3
  Serial1.write(0x10);
  */
  Serial1.write(crc);
  while(Serial1.available())  Serial.print(Serial1.read());
  //Serial.println("Serial1 cleared");
  Serial1.write(crc>>8);
  while(!Serial1.available());
  char c2 = Serial1.read();
  Serial.print("CRC result ! : ");
  Serial.print(c2);
  Serial.print(" -- ");
  Serial.println(c2, HEX);
  if(c2 != 'O')
    while(1);
  Serial.println("DONE");
  Serial.println("sent whole frame !");

  while(!Serial1.available());
  //Serial.println("recd");
  if(Serial1.read()==0) {
    //Serial.println("got 0");
    Serial1.write('O');
    //Serial.println("sent ok");
  
    while(!Serial1.available());
    //Serial.println("Got 2");
    while(Serial1.available()) {
      Serial.println(Serial1.read(), HEX);
    } // 1 0 4 5 - Client / server command specifier not valid or unknown

   
  }
  delay(3000);

  
  t = e.get_error_register_bits();
  Serial.print("E - error register : ");
  Serial.println(t, BIN);

  t = e.get_ipm_buffer_status_bits();
  Serial.print("E - IPM buffer status : ");
  Serial.println(t, BIN);
  
  t = e.get_ipm_maximum_buffer_size();
  Serial.print("E - ipm buffer : max=");
  Serial.print(t);
  t = e.get_ipm_actual_buffer_size();
  Serial.print("    actual=");
  Serial.print(t);
  t = e.get_ipm_buffer_position();
  Serial.print("    pos=");
  Serial.println(t);
  while(1);
}  
  t = e.get_controlword_bits();
  Serial.print("E - control word : ");
  Serial.print(t, BIN);
  t = e.set_controlword(0x1F);
  Serial.print(" --> Start IPM --> ");  
  t = e.get_controlword_bits();
  Serial.println(t, BIN);

  t = e.get_ipm_buffer_status_bits();
  Serial.print("E - IPM buffer status : ");
  Serial.println(t, BIN);

  t = e.get_statusword_bits();
  Serial.print("E - status word 4 : ");
  Serial.println(t, BIN);

  t = e.get_error_register_bits();
  Serial.print("E - error register : ");
  Serial.println(t, BIN);
  
  while(1) {
    if( Serial.available() )
      break;
    Serial.print("E - position : ");
    t = e.get_position_demand_value();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_position_actual_value();
    Serial.print("    actual= ");
    Serial.println(t);
    Serial.print("E - velocity : ");
    t = e.get_velocity_demand_value();
    Serial.print("    demand= ");
    Serial.print(t);
    t = e.get_velocity_actual_value();
    Serial.print("    actual= ");
    Serial.println(t);
    t = e.get_ipm_buffer_status_bits();
    Serial.print("E - IPM buffer status : ");
    Serial.println(t, BIN);
  }
  
  t = e.get_statusword_bits();
  Serial.print("E - status word 5 : ");
  Serial.println(t, BIN);





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



