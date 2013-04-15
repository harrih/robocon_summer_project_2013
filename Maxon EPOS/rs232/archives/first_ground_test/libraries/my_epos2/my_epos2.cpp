# include "my_epos2.h"
//# define EPOS2_DEBUG Serial // for debugging into Serial

// --------------------------------------------------------------------
// Constructor Destructor
//---------------------------------------------------------------------
EPOS2::EPOS2 () {
}
EPOS2::EPOS2 (HardwareSerial *s, unsigned short n) {
	init(s, n);
}
EPOS2::EPOS2 (HardwareSerial *s, unsigned short n, EPOS2_MOTOR_ROTATIONS d) {
	init(s, n, d);
}

void EPOS2::init (HardwareSerial *s, unsigned short n) {
	int32_t t;
	serial_ = s;
	nodeId = n;
	
	serial_->begin(EPOS2_BAUDRATE);
	serial_flush();
# ifdef EPOS2_DEBUG
	EPOS2_DEBUG.println("Serial initiated for epos2");
# endif
	disable();
	t=set_motor_type(DC_MOTOR);
	if(t) {
		Serial.println("WARNING : Couldn't set motor to DC");
	}
	t=set_rs232_frame_timeout(100);
	if(t) {
		Serial.println("WARNING : Couldn't set rs232 timeout");
	}
	t=set_max_acceleration(4000);
	if(t) {
		Serial.println("WARNING : Couldn't set max acceleration");
	}
	t=set_maximal_profile_velocity(4000);
	if(t) {
		Serial.println("WARNING : Couldn't set maximal profile velocity");
	}
	t=set_maximal_following_error(40000);
	if(t) {
		Serial.println("WARNING : Couldn't set maximal following error");
	}
	t=set_gear_ratio_numerator(1);
	if(t) {
		Serial.println("WARNING : Couldn't set Gear ratio Numerator");
	}
	t=set_gear_ratio_denominator(21);
	if(t) {
		Serial.println("WARNING : Couldn't set Gear ratio Numerator");
	}
}
void EPOS2::init(HardwareSerial *s, unsigned short n, EPOS2_MOTOR_ROTATIONS d) {
  init(s, n);
  rotation(d);
}

void EPOS2::enable() {
	int32_t t;
	t = set_controlword(0x0F); // switch_on, enable_voltage, quickstop, enable_operation
	if(t) {
		Serial.println("ERROR: Couldn't start epos2.");
		kill();
	}
}
void EPOS2::disable() {
	int32_t t;
	t = set_controlword(0x06);
	if(t) {
		Serial.println("ERROR: Couldn't switch off epos2.");
		kill();
	}
}

// --------------------------------------------------------------------
// General communication functions
//---------------------------------------------------------------------

void EPOS2::serial_flush () { // Clears Serial buffer
	 while(serial_->available())
		serial_->read();
}

int EPOS2::wait_for_serial () { // Waits for Serial until timeout(500)
	unsigned long int pmillis = millis();
	while(!serial_->available())
		if(millis()-pmillis > 500)
			return 1;
	return 0;
}

unsigned int EPOS2::get_crc (epos2_frame f) { // Gets the CRC for EPOS2 frame
	//Calculate pDataArray Word by Word
	//Serial.println("function called");
	int data[6] = { (f.opcode<<8) + f.len, f.data[0], f.data[1], f.data[2], f.data[3], 0 };
	data[f.len + 2] = 0; // reset the last array element to 0 (in case random values are there ...
	int numberOfWords = f.len + 3; // the number_of_words (=len+1) + the 0 + opcode/len
	//unsigned int opcode = f.opcode;
	
	unsigned int shifter, c;
	unsigned int carry;
	unsigned int CRC = 0 , i = 0;
	while(numberOfWords--) {
		//Serial.println("In while loop");
		shifter = 0x8000;
		c = data[i++];
		do {
			//Initialize BitX to Bit15
			//Copy next DataWord to c
			carry = CRC & 0x8000;
			CRC <<= 1;
			if(c & shifter) CRC++;
			if(carry) CRC ^= 0x1021;
			shifter >>= 1;
			//Serial.println(shifter);
		} while(shifter);
		//Serial.println("Do while completed");
	}  
  //Serial.println(CRC);
  return CRC;
}
unsigned int EPOS2::get_crc (int data[], int numberOfWords) { // Gets the CRC for an array
	//Calculate pDataArray Word by Word
	//Serial.println("function called");
	/*
	 * int data[6] = { (f.opcode<<8) + f.len, f.data[0], f.data[1], f.data[2], f.data[3], 0 };
	 * data[f.len + 2] = 0; // reset the last array element to 0 (in case random values are there ...
	 * int numberOfWords = f.len + 3; // the number_of_words (=len+1) + the 0 + opcode/len
	 * unsigned int opcode = f.opcode;
	 */
	
	unsigned int shifter, c;
	unsigned int carry;
	unsigned int CRC = 0 , i = 0;
	while(numberOfWords--) {
		//Serial.println("In while loop");
		shifter = 0x8000;
		c = data[i++];
		do {
			//Initialize BitX to Bit15
			//Copy next DataWord to c
			carry = CRC & 0x8000;
			CRC <<= 1;
			if(c & shifter) CRC++;
			if(carry) CRC ^= 0x1021;
			shifter >>= 1;
			//Serial.println(shifter);
		} while(shifter);
		//Serial.println("Do while completed");
	}  
  //Serial.println(CRC);
  return CRC;
}

int EPOS2::send (epos2_frame f) { // Sends the frame to epos, and handles complete comm for it
	int counter_t = 0;
	char c, c2;
	f.data[1] = (nodeId<<8) + (f.data[1] & 0x00ff);
	set_crc(&f);
	do {
		do {
			if(counter_t++ > 5)
				return 3;
			serial_flush();
			# ifdef EPOS2_DEBUG
				EPOS2_DEBUG.println("Sending ... ");
				EPOS2_DEBUG.println(f.opcode);
			# endif
			
			serial_flush(); // flush it so after sending don't get garbage values
			serial_->write(f.opcode);
		} while(wait_for_serial());
		c = serial_->read();
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("Opcode Sent was : ");
			EPOS2_DEBUG.println(f.opcode, HEX);
			if( c == 'O' )
				EPOS2_DEBUG.println("Opcode return was OK");
			else if( c == 'F' ) 
				EPOS2_DEBUG.println("Opcode return was Fail");
			else
				EPOS2_DEBUG.println("Opcode return was weird");
			EPOS2_DEBUG.println("Sending frame :");
			print_epos2_frame(f);
		#endif
		if( c == 'O' ) {
			serial_->write(f.len);
			int i = 0;
			for( i = 0; i <= f.len; i++ ) {
				serial_->write(f.data[i]);
				serial_->write(f.data[i]>>8);
			}
			serial_->write(f.crc);
			serial_flush(); // flush it so after sending don't get garbage values
			serial_->write(f.crc>>8);
		}
	} while(wait_for_serial());
	if( c == 'O' ) {
		c2 = serial_->read();
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("CRC Sent was : ");
			EPOS2_DEBUG.println(f.crc, HEX);
			if( c2 == 'O' || c == 'o' )
				EPOS2_DEBUG.println("CRC return was OK");
			else if( c2 == 'f' || c2 == 'F' ) 
				EPOS2_DEBUG.println("CRC return was Fail");
			else
				EPOS2_DEBUG.println("CRC return was weird");
		#endif
		if( c2 == 'O' ) {
			return 0;
		} else if( c2 == 'F' ) {
			# ifdef EPOS2_DEBUG	
				EPOS2_DEBUG.print("Couldn't send error got was: ");
				EPOS2_DEBUG.println(2);
			# endif
			return 2;
		} else {
			# ifdef EPOS2_DEBUG	
				EPOS2_DEBUG.print("Couldn't send error got was: ");
				EPOS2_DEBUG.println(-2);
			# endif
			return -2;
		}
	} else if( c == 'F' ) {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(1);
		# endif
		return 1;
	} else {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(-1);
		# endif
		return -1;
	}
}
int EPOS2::send_data (epos2_frame f) { // Sends the frame to epos, and handles complete comm for it
	int counter_t = 0;
	char c, c2;
	f.data[1] = (nodeId<<8) + (f.data[1] & 0x00ff);
	set_crc(&f);
	
	do {
		if(counter_t++ > 5)
			return 3;
		serial_flush();
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.println("Sending ... ");
			EPOS2_DEBUG.println(f.opcode);
		# endif
		
		serial_flush(); // flush it so after sending don't get garbage values
		serial_->write(f.opcode);
	} while(wait_for_serial());
	c = serial_->read();
	# ifdef EPOS2_DEBUG
		EPOS2_DEBUG.print("Opcode Sent was : ");
		EPOS2_DEBUG.println(f.opcode, HEX);
		if( c == 'O' )
			EPOS2_DEBUG.println("Opcode return was OK");
		else if( c == 'F' ) 
			EPOS2_DEBUG.println("Opcode return was Fail");
		else
			EPOS2_DEBUG.println("Opcode return was weird");
		EPOS2_DEBUG.println("Sending frame :");
		print_epos2_frame(f);
	#endif
	if( c == 'O' ) {
		serial_->write(f.len);
		int i = 0;
		for( i = 0; i <= f.len; i++ ) {
			serial_->write(f.data[i]);
			serial_->write(f.data[i]>>8);
		}
		serial_->write(f.crc);
		serial_flush(); // flush it so after sending don't get garbage values
		return 0;
	} else if( c == 'F' ) {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(1);
		# endif
		return 1;
	} else {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(-1);
		# endif
		return -1;
	}
}
int EPOS2::send_crc (epos2_frame f) { // Sends the frame to epos, and handles complete comm for it
	int counter_t = 0;
	char c, c2;
	f.data[1] = (nodeId<<8) + (f.data[1] & 0x00ff);
	set_crc(&f);
	do {
		serial_flush(); // flush it so after sending don't get garbage values
		serial_->write(f.crc>>8);
	} while(wait_for_serial());
	c2 = serial_->read();
	# ifdef EPOS2_DEBUG
		EPOS2_DEBUG.print("CRC Sent was : ");
		EPOS2_DEBUG.println(f.crc, HEX);
		if( c2 == 'O' || c == 'o' )
			EPOS2_DEBUG.println("CRC return was OK");
		else if( c2 == 'f' || c2 == 'F' ) 
			EPOS2_DEBUG.println("CRC return was Fail");
		else
			EPOS2_DEBUG.println("CRC return was weird");
	#endif
	if( c2 == 'O' ) {
		return 0;
	} else if( c2 == 'F' ) {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(2);
		# endif
		return 2;
	} else {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(-2);
		# endif
		return -2;
	}
}
int EPOS2::send_segments (uint16_t ls[], int len) { // Sends the segmented frame to epos, and handles complete comm for it
	int counter_t = 0;
	char c, c2;
	int data[len+3];
	data[0] = (SEGMENT_WRITE_OPCODE<<8) + (len-1);
	data[1] = ((len)-1) + (1<<6); // <<6 => Toggle Bit
	for(int i = 0; i < len; i++)
		data[i+2] = ls[i];
	data[len+2] = 0;
	uint16_t crc = get_crc(data, len+3); // get crc
	do {
		do {
			if(counter_t++ > 5)
				return 3;
			serial_flush();
			# ifdef EPOS2_DEBUG
				EPOS2_DEBUG.println("Sending ... ");
				EPOS2_DEBUG.println(SEGMENT_WRITE_OPCODE);
			# endif
			
			serial_flush(); // flush it so after sending don't get garbage values
			serial_->write(SEGMENT_WRITE_OPCODE);
		} while(wait_for_serial());
		c = serial_->read();
		//Serial.println("DEBUG");
		//Serial.print(" ... for opcode : ");
		//Serial.println(c);
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("Opcode Sent was : ");
			EPOS2_DEBUG.println(SEGMENT_WRITE_OPCODE, HEX);
			if( c == 'O' )
				EPOS2_DEBUG.println("Opcode return was OK");
			else if( c == 'F' ) 
				EPOS2_DEBUG.println("Opcode return was Fail");
			else
				EPOS2_DEBUG.println("Opcode return was weird");
			EPOS2_DEBUG.println("Sending frame segments.");
			//print_epos2_frame(f);
		#endif
		if( c == 'O' ) {
			serial_->write(len-1);
			int i = 0;
			serial_->write(data[1]); // sending this weird thing called control Byte
			serial_->write(data[1]>>8);
			for( i = 0; i < len; i++ ) { // sending data
				serial_->write(data[i+2]);
				serial_->write(data[i+2]>>8);
			}
			serial_->write(crc);
			serial_flush(); // flush it so after sending don't get garbage values
			serial_->write(crc>>8);
		}
	} while(wait_for_serial());
	if( c == 'O' ) {
		c2 = serial_->read();
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("CRC Sent was : ");
			EPOS2_DEBUG.println(crc, HEX);
			if( c2 == 'O' || c == 'o' )
				EPOS2_DEBUG.println("CRC return was OK");
			else if( c2 == 'f' || c2 == 'F' ) 
				EPOS2_DEBUG.println("CRC return was Fail");
			else
				EPOS2_DEBUG.println("CRC return was weird");
		#endif
		//Serial.print(" ... for crc : ");
		//Serial.println(c);
		if( c2 == 'O' ) {
			return 0;
		} else if( c2 == 'F' ) {
			# ifdef EPOS2_DEBUG	
				EPOS2_DEBUG.print("Couldn't send error got was: ");
				EPOS2_DEBUG.println(2);
			# endif
			return 2;
		} else {
			# ifdef EPOS2_DEBUG	
				EPOS2_DEBUG.print("Couldn't send error got was: ");
				EPOS2_DEBUG.println(-2);
			# endif
			return -2;
		}
	} else if( c == 'F' ) {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(1);
		# endif
		return 1;
	} else {
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(-1);
		# endif
		return -1;
	}
}

int EPOS2::get (epos2_frame* f) { // Gets a frame from epos, and handles complete comm for it
	int count_t = 0;
	unsigned short temps;
	unsigned int temp;
	do {
		do {
			do {
				do {
					do {
						do {
							do {
								do {
									do {
										do {
											do {
												do {
													if(count_t++ > 5)
														return 2;
												
													# ifdef EPOS2_DEBUG
														EPOS2_DEBUG.println("Trying to receive.");
													# endif
													
												} while(wait_for_serial());
												temps = serial_->read();
												f->opcode = temps;
												# ifdef EPOS2_DEBUG
													EPOS2_DEBUG.println("Got opcode.");
												# endif
												serial_flush(); // so garbage doesn't affect my inputs
												serial_->write('O'); // tell epos I recd. opcode !
											} while(wait_for_serial());
											temps = serial_->read();
											f->len = temps;
											# ifdef EPOS2_DEBUG
												EPOS2_DEBUG.println("Got length.");
											# endif
											
										} while(wait_for_serial());
										temp = serial_->read();
									} while(wait_for_serial());
							
									temp += (serial_->read() << 8);
									f->data[0] = temp;
									# ifdef EPOS2_DEBUG
										EPOS2_DEBUG.println("Got data 0.");
									# endif
									
								} while(wait_for_serial());
								temp = serial_->read();
							} while(wait_for_serial());
							temp += (serial_->read() << 8);
							f->data[1] = temp;
							# ifdef EPOS2_DEBUG
								EPOS2_DEBUG.println("Got data 1.");
							# endif
							
						} while(wait_for_serial());
						temp = serial_->read();
					} while(wait_for_serial());
					temp += (serial_->read() << 8);
					f->data[2] = temp;
					# ifdef EPOS2_DEBUG
						EPOS2_DEBUG.println("Got data 2.");
					# endif
					
				} while(wait_for_serial());
				temp = serial_->read();
			} while(wait_for_serial());
			temp += (serial_->read() << 8);
			f->data[3] = temp;
			# ifdef EPOS2_DEBUG
				EPOS2_DEBUG.println("Got data 3.");
			# endif
			
		} while(wait_for_serial());
		temp = serial_->read();
	} while(wait_for_serial());
	temp += (serial_->read() << 8);
	f->crc = temp;
	# ifdef EPOS2_DEBUG
		EPOS2_DEBUG.println("Got CRC.");
	# endif
	if( check_crc(*f) ) {
		# ifdef EPOS2_DEBUG
			print_epos2_frame(*f);
		# endif
		return 0;
	} else { // what to do if i get wrong crc
		# ifdef EPOS2_DEBUG	
			EPOS2_DEBUG.print("Couldn't send error got was: ");
			EPOS2_DEBUG.println(1);
		# endif
		return 1;
	}
}

void print(char a[]) {
	Serial.print(a);
}
void print(int16_t a) {
	Serial.print(a);
}
void print(int32_t a) {
	Serial.print(a);
}
void println(char a[]) {
	Serial.println(a);
}
void println(int16_t a) {
	Serial.println(a);
}
void println(int32_t a) {
	Serial.println(a);
}
void println() {
	Serial.println();
}

// --------------------------------------------------------------------
// General device constants
//---------------------------------------------------------------------

uint32_t EPOS2::get_device_type() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x1000, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

uint16_t EPOS2::get_software_version() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2003, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

uint16_t EPOS2::get_hardware_version() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2003, 0x02, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

uint32_t EPOS2::get_supported_drive_modes() {
	/*	bit val description
		31...22 0 reserved
		21 1 Maxon Step/Direction Mode
		20 1 Maxon Master Encoder Mode
		19 1 Maxon Diagnostic Mode
		18 1 Maxon Current Mode
		17 1 Maxon Velocity Mode
		16 1 Maxon Position Mode
		15...7 0 reserved
		6 1 Interpolated Position Mode
		5 1 Homing Mode
		4 0 reserved
		3 0 (Torque Mode, not implemented)
		2 1 Profile Velocity Mode
		1 0 (Velocity Mode, not implemented)
		0 1 Profile Position Mode
	 */
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6502, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

// --------------------------------------------------------------------
// Device Settings
//---------------------------------------------------------------------

uint16_t EPOS2::get_rs232_frame_timeout() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2005, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_rs232_frame_timeout(uint16_t ti) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x2005, 0x00, (uint16_t) ti, 0));
	if(t) return -1;
	return(0);
}

epos2_controlword EPOS2::get_controlword() {
	int t;
	epos2_controlword cw;
		cw.switch_on = -1;
		cw.enable_voltage = -1;
		cw.quickstop = -1;
		cw.enable_operation = -1;
		cw.mode_enable = -1;
		cw.mode_change_immediate = -1;
		cw.mode_absolute = -1;
		cw.fault_reset = -1;
		cw.mode_halt = -1;
	t = send(get_frame(READ_OPCODE, 1, 0x6040, 0x00, 0, 0));
	if(t) return cw;
	epos2_frame f;
	t = get(&f);
	if(t) return cw;
	uint16_t d = f.data[2];
			cw.switch_on = d%2;
	d>>1;	cw.enable_voltage = d%2;
	d>>1;	cw.quickstop = d%2;
	d>>1;	cw.enable_operation = d%2;
	d>>1;	cw.mode_enable = d%2;
	d>>1;	cw.mode_change_immediate = d%2;
	d>>1;	cw.mode_absolute = d%2;
	d>>1;	cw.fault_reset = d%2;
	d>>1;	cw.mode_halt = d%2;
	
	return(cw);
}
uint16_t EPOS2::get_controlword_bits() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6040, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_controlword(uint16_t w) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6040, 0x00, (uint16_t) w, 0));
	if(t) return -1;
	return(0);
}
int EPOS2::set_controlword(epos2_controlword cw) {
	int t;
	uint16_t w = cw.switch_on ||
			cw.enable_voltage<<1;
			cw.quickstop<<2;
			cw.enable_operation<<3;
			cw.mode_enable<<4;
			cw.mode_change_immediate<<5;
			cw.mode_absolute<<6;
			cw.fault_reset<<7;
			cw.mode_halt<<8;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6040, 0x00, (uint16_t) w, 0));
	if(t) return -1;
	return(0);
}

epos2_statusword EPOS2::get_statusword() {
	int t;
	epos2_statusword sw;
		sw.ready_to_switch_on = -1;
		sw.switched_on = -1;
		sw.operation_enable = -1;
		sw.fault = -1;
		sw.voltage_enabled = -1;
		sw.quickstop = -1;
		sw.switch_on_disable = -1;
		sw.warning = -1;
		sw.offset_current_measured = -1;
		sw.remote = -1;
		sw.mode_target_reached = -1;
		sw.internal_limit_active = -1;
		sw.mode_active = -1;
		sw.mode_error = -1;
		sw.refresh_cycle_of_power_stage = -1;
		sw.positon_referenced_to_home_position = -1;
	t = send(get_frame(READ_OPCODE, 1, 0x6041, 0x00, 0, 0));
	if(t) return sw;
	epos2_frame f;
	t = get(&f);
	if(t) return sw;
	uint16_t d = f.data[2];
			sw.ready_to_switch_on = d%2;
	d>>1;	sw.switched_on = d%2;
	d>>1;	sw.operation_enable = d%2;
	d>>1;	sw.fault = d%2;
	d>>1;	sw.voltage_enabled = d%2;
	d>>1;	sw.quickstop = d%2;
	d>>1;	sw.switch_on_disable = d%2;
	d>>1;	sw.warning = d%2;
	d>>1;	sw.offset_current_measured = d%2;
	d>>1;	sw.remote = d%2;
	d>>1;	sw.mode_target_reached = d%2;
	d>>1;	sw.internal_limit_active = d%2;
	d>>1;	sw.mode_active = d%2;
	d>>1;	sw.mode_error = d%2;
	d>>1;	sw.refresh_cycle_of_power_stage = d%2;
	d>>1;	sw.positon_referenced_to_home_position = d%2;
	return(sw);
}
uint16_t EPOS2::get_statusword_bits() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6041, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

uint16_t EPOS2::get_motor_type() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6402, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_motor_type(MOTOR_TYPES ty) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6402, 0x00, (uint16_t) ty, 0));
	if(t) return -1;
	return(0);
}

epos2_miscconfig EPOS2::get_misc_config() {
	int t;
	epos2_miscconfig mc;
		mc.sensor_supervision_by_software = -1; 
		mc.sensor_supervision_by_hardware = -1;
		mc.measure_dc_motor_resistance = -1;
		mc.measure_motor_speed_with_main_encoder = -1;
		mc.measure_motor_speed_with_auxiliary_encoder = -1;
		mc.polarity_bit = -1;
	t = send(get_frame(READ_OPCODE, 1, 0x2008, 0x00, 0, 0));
	if(t) return mc;
	epos2_frame f;
	t = get(&f);
	if(t) return mc;
	uint16_t d = f.data[2];
			mc.sensor_supervision_by_software = d%2; 
	d>>1;	mc.sensor_supervision_by_hardware = d%2;
	d>>1;	mc.measure_dc_motor_resistance = d%2;
	d>>1;	mc.measure_motor_speed_with_main_encoder = d%2;
	d>>1;	mc.measure_motor_speed_with_auxiliary_encoder = d%2;
	d>>4;	mc.polarity_bit = d%2;
	
	return(mc);
}
uint16_t EPOS2::get_misc_config_bits() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2008, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_misc_config(epos2_miscconfig mc) {
	int t;
	uint16_t c = mc.sensor_supervision_by_software || 
			mc.sensor_supervision_by_hardware<<1 ||
			mc.measure_dc_motor_resistance<<2 ||
			mc.measure_motor_speed_with_main_encoder<<3 ||
			mc.measure_motor_speed_with_auxiliary_encoder<<4 ||
			mc.polarity_bit<<8 ;
	t = send(get_frame(WRITE_OPCODE, 2, 0x2008, 0x00, (uint16_t) c, 0));
	if(t) return -1;
	return(0);
}
int EPOS2::set_misc_config(uint16_t c) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x2008, 0x00, (uint16_t) c, 0));
	if(t) return -1;
	return(0);
}

epos2_error_frame EPOS2::get_error_register() {
	int t;
	epos2_error_frame ef;
		ef.generic = -1;
		ef.current = -1;
		ef.voltage = -1;
		ef.temperature = -1;
		ef.communication = -1;
		ef.device_profile_specific = -1;
		ef.motion_error = -1;
	t = send(get_frame(READ_OPCODE, 1, 0x1001, 0x00, 0, 0));
	if(t) return ef;
	epos2_frame f;
	t = get(&f);
	if(t) return ef;
	uint16_t d = f.data[2];
	d>>1;	ef.generic = d%2;
	d>>1;	ef.current = d%2;
	d>>1;	ef.voltage = d%2;
	d>>1;	ef.temperature = d%2;
	d>>1;	ef.communication = d%2;
	d>>1;	ef.device_profile_specific = d%2;
	d>>2;	ef.motion_error = d%2;
	return(ef);
}
uint8_t EPOS2::get_error_register_bits() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x1001, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

uint32_t EPOS2::get_gear_ratio_numerator() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2230, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_gear_ratio_numerator(uint32_t gn) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x2230, 0x01, (uint16_t) gn, (uint16_t) gn>>16));
	if(t) return -1;
	return(0);
}

uint16_t EPOS2::get_gear_ratio_denominator() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2230, 0x02, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_gear_ratio_denominator(uint16_t gd) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x2230, 0x02, (uint16_t) gd, 0));
	if(t) return -1;
	return(0);
}



// --------------------------------------------------------------------
// Operation Mode Settings
//---------------------------------------------------------------------

	// ----------------------------------------------------------------
	// General
	//-----------------------------------------------------------------
int8_t EPOS2::get_operation_mode() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6061, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((int8_t) f.data[2]);
}
int EPOS2::set_operation_mode(OPERATION_MODES m) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6060, 0x00, m, 0));
	if(t) return -1;
	return(0);
}

int EPOS2::set_motion_profile_type(MOTION_PROFILES ty) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6086, 0x00, (uint16_t) ty, 0));
	if(t) return -1;
	return(0);
}
uint16_t EPOS2::get_motion_profile_type() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6086, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

	// ----------------------------------------------------------------
	// Acceleration
	//-----------------------------------------------------------------
	
uint32_t EPOS2::get_max_acceleration() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x60C5, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_max_acceleration(uint32_t a) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x60C5, 0x00, (uint16_t) a, (uint16_t) a>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_profile_acceleration() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6083, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_profile_acceleration(uint32_t a) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6083, 0x00, (uint16_t) a, (uint16_t) a>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_profile_deceleration() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6084, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_profile_deceleration(uint32_t d) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6084, 0x00, (uint16_t) d, (uint16_t) d>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_quickstop_deceleration() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6085, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_quickstop_deceleration(uint32_t d) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6085, 0x00, (uint16_t) d, (uint16_t) d>>16));
	if(t) return -1;
	return(0);
}

	// ----------------------------------------------------------------
	// Velocity
	//-----------------------------------------------------------------
	
uint32_t EPOS2::get_maximal_profile_velocity() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607F, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_maximal_profile_velocity(uint32_t v) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607F, 0x00, (uint16_t) v, (uint16_t) v>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_velocity_mode_setting_value() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x206B, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_velocity_mode_setting_value(int32_t v) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x206B, 0x00, (uint16_t) v, (uint16_t) v>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_velocity_actual_value() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x606C, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_velocity_demand_value() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x606B, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_target_velocity() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x60FF, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_target_velocity(int32_t v) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x60FF, 0x00, (uint16_t) v, (uint16_t) v>>16));
	if(t) return -1;
	return(0);
}

int EPOS2::set_profile_velocity(uint32_t v) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6081, 0x00, (uint16_t) v, (uint16_t) v>>16));
	if(t) return -1;
	return(0);
}
uint32_t EPOS2::get_profile_velocity() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6081, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

uint32_t EPOS2::get_velocity_window() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x606D, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_velocity_window(uint32_t w) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x606D, 0x00, (uint16_t) w, (uint16_t) w>>16));
	if(t) return -1;
	return(0);
}

uint16_t EPOS2::get_velocity_window_time() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x606E, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_velocity_window_time(uint16_t ti) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x606E, 0x00, (uint16_t) ti, 0));
	if(t) return -1;
	return(0);
}

	// ----------------------------------------------------------------
	// Position
	//-----------------------------------------------------------------

int32_t EPOS2::get_maximal_position_limit() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607D, 0x02, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_maximal_position_limit(int32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607D, 0x02, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_minimal_position_limit() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607D, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_minimal_position_limit(int32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607D, 0x01, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_maximal_following_error() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6065, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_maximal_following_error(uint32_t dp) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6065, 0x00, (uint16_t) dp, (uint16_t) dp>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_position_mode_setting_value() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2062, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_position_mode_setting_value(int32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x2062, 0x00, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_position_actual_value() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6064, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_position_demand_value() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6062, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_target_position() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607A, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_target_position(int32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607A, 0x00, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_position_window() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6067, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}
int EPOS2::set_position_window(uint32_t w) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6067, 0x00, (uint16_t) w, (uint16_t) w>>16));
	if(t) return -1;
	return(0);
}

uint16_t EPOS2::get_position_window_time() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6068, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_position_window_time(uint16_t ti) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6068, 0x00, (uint16_t) ti, 0));
	if(t) return -1;
	return(0);
}

	// ----------------------------------------------------------------
	// Interpolated Position Mode
	//-----------------------------------------------------------------

uint32_t EPOS2::get_ipm_maximum_buffer_size() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x60C4, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

uint32_t EPOS2::get_ipm_actual_buffer_size() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x60C4, 0x02, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

uint16_t EPOS2::get_ipm_buffer_position() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x60C4, 0x04, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

int EPOS2::set_ipm_buffer_clear(uint8_t v) { 
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x60C4, 0x06, (uint16_t) v, 0));
	if(t) return -1;
	return(0);
}

epos2_ipm_status EPOS2::get_ipm_buffer_status() {
	int t;
	epos2_ipm_status is;
		is.underflow_warning = -1;
		is.overflow_warning = -1;
		is.velocity_warning = -1;
		is.acceleration_warning = -1;
		is.underflow_error = -1;
		is.overflow_error = -1;
		is.velocity_error = -1;
		is.acceleration_error = -1;
		is.buffer_enabled = -1;
		is.ip_mode_active = -1;
	t = send(get_frame(READ_OPCODE, 1, 0x20C4, 0x01, 0, 0));
	if(t) return is;
	epos2_frame f;
	t = get(&f);
	if(t) return is;
	uint16_t d = f.data[2];
			is.underflow_warning = d%2;
	d>>1;	is.overflow_warning = d%2;
	d>>1;	is.velocity_warning = d%2;
	d>>1;	is.acceleration_warning = d%2;
	d>>5;	is.underflow_error = d%2;
	d>>1;	is.overflow_error = d%2;
	d>>1;	is.velocity_error = d%2;
	d>>1;	is.acceleration_error = d%2;
	d>>3;	is.buffer_enabled = d%2;
	d>>1;	is.ip_mode_active = d%2;
	return(is);
}
uint16_t EPOS2::get_ipm_buffer_status_bits() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x20C4, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

uint16_t EPOS2::get_ipm_buffer_underflow_warning() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x20C4, 0x02, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_ipm_buffer_underflow_warning(uint16_t l) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x20C4, 0x02, (uint16_t) l, 0));
	if(t) return -1;
	return(0);
}

uint16_t EPOS2::get_ipm_buffer_overflow_warning() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x20C4, 0x03, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}
int EPOS2::set_ipm_buffer_overflow_warning(uint16_t l) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x20C4, 0x03, (uint16_t) l, 0));
	if(t) return -1;
	return(0);
}

int EPOS2::add_to_ipm_buffer(epos2_ipm_frame fs[], int len) {
	int t;
	t = send(get_frame(INIT_SEGMENT_WRITE_OPCODE, 3, 0x20C1, 0x00, (uint16_t) len*8, 0)); // total number of bytes to write ... is it 16bit or 8bit ?
	if(t) return -1;
	uint16_t ls[4*len];
	for( int i = 0; i < len; i++ ) { // make the array to send
		ls[4*i] = fs[i].p;
		ls[4*i+1] = fs[i].p>>16;
		ls[4*i+2] = (fs[i].v&0xffff);
		ls[4*i+3] = ((fs[i].v>>16)&0x00ff) + (fs[i].t<<8);
	}
	t = send_segments(ls, 4*len);
	if(t) return -1;
	//t = send(get_frame(WRITE_OPCODE, 3, 0x20C1, 0x00, (uint16_t) p, (uint16_t) ( (t<<24) + (v&0x00ffffff))));
	//if(t) return -1;
	return(0);
}

// --------------------------------------------------------------------
// EPOS2_Frame functions
//---------------------------------------------------------------------

epos2_frame get_frame(uint8_t o, uint8_t l, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3) {
	epos2_frame f;
	f.opcode = o;
	f.len = l;
	f.data[0] = d0;
	f.data[1] = (d1 & 0x00ff); // this is to mask and remove any node ID given
	f.data[2] = d2;
	f.data[3] = d3;
	return f;
}

void print_epos2_frame(epos2_frame f) {
# ifdef EPOS2_DEBUG  
	EPOS2_DEBUG.print( " Opcode : " );
	EPOS2_DEBUG.println( f.opcode, HEX );
	EPOS2_DEBUG.print( " Length : " );
	EPOS2_DEBUG.println( f.len, HEX );
	EPOS2_DEBUG.print( " data 0 : " );
	EPOS2_DEBUG.println( f.data[0], HEX );
	EPOS2_DEBUG.print( " data 1 : " );
	EPOS2_DEBUG.println( f.data[1], HEX );
	EPOS2_DEBUG.print( " data 2 : " );
	EPOS2_DEBUG.println( f.data[2], HEX );
	EPOS2_DEBUG.print( " data 3 : " );
	EPOS2_DEBUG.println( f.data[3], HEX );
# endif
}

int EPOS2::check_crc (epos2_frame f) {
	return (f.crc == get_crc(f));
}

void EPOS2::set_crc (epos2_frame* f) { // Sets the CRC for EPOS2 Frame
	unsigned int crc = get_crc(*f);
	f->crc = crc;
}

// --------------------------------------------------------------------
// Nice functions   ==   NICE FUNCTIONS   ==   Nice Functions
//---------------------------------------------------------------------

void EPOS2::kill() {
	int32_t t;
	t = set_controlword(0x000B);
	if(t) {
		Serial.println("ERROR: failed to quickstop o.O.");
		kill();
		return;
	}
while(1);
}

void EPOS2::rotation(EPOS2_MOTOR_ROTATIONS d) {
  if( d == CLOCKWISE_ROTATION )
    set_misc_config(0x0100); // clockwise
  else if( d == ANTICLOCKWISE_ROTATION)
    set_misc_config(0x0000); // anticlockwise
}

void EPOS2::goto_position(int32_t p, uint32_t v, uint32_t a) {
	int32_t t;disable();
	t = set_max_acceleration(a);
	if(t) {
		Serial.println("WARNING : Couldn't set max acceleration");
	}
	t = set_maximal_profile_velocity(v);
	if(t) {
		Serial.println("WARNING : Couldn't set maximal profile velocity");
	}
	goto_position(p);
}
void EPOS2::goto_position(int32_t p, uint32_t v) {
	int32_t t;
	disable();
	t = set_maximal_profile_velocity(v);
	if(t) {
		Serial.println("WARNING : Couldn't set maximal profile velocity");
	}
	goto_position(p);
}
void EPOS2::goto_position(int32_t p) {
	int32_t t;
	disable();
	t = set_operation_mode(POSITION_MODE);
	if(t) {
		Serial.println("ERROR: failed to set mode to Position mode.");
		kill();
		return;
	}
	enable();
	t = set_position_mode_setting_value(p);
	if(t) {
		Serial.println("ERROR: failed to set mode to Position mode.");
		kill();
		return;
	}
}

void EPOS2::position(int32_t p, uint32_t v, uint32_t a, uint32_t d, uint32_t qd, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(d);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(qd);
	if(t) {
		Serial.println("ERROR: failed to set quickstop Deceleration.");
		kill();
		return;
	}
	position(p, v, m);
}
void EPOS2::position(int32_t p, uint32_t v, uint32_t a, uint32_t d, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(d);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(d*3);
	if(t) {
		Serial.println("ERROR: failed to set quickstop Deceleration.");
		kill();
		return;
	}
	position(p, v, m);
}
void EPOS2::position(int32_t p, uint32_t v, uint32_t a, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(a*3);
	if(t) {
		Serial.println("ERROR: failed to set Quickstop Deceleration.");
		kill();
		return;
	}
	position(p, v, m);
}
void EPOS2::position(int32_t p, uint32_t v, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_velocity(v);
	if(t) {
		Serial.println("ERROR: failed to set Profile Velocity.");
		kill();
		return;
	}
	position(p, m);
}
void EPOS2::position(int32_t p, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_operation_mode(PROFILE_POSITION_MODE);
	if(t) {
		Serial.println("ERROR: failed to set mode to Profile Position mode.");
		kill();
		return;
	}
	
	t = set_motion_profile_type(m);
	if(t) {
		Serial.println("ERROR: failed to set Motion Profile Type.");
		kill();
		return;
	}
	
//	enable();
	t = set_target_position(p);
	if(t) {
		Serial.println("ERROR: failed to set target position.");
		kill();
		return;
	}
}
void EPOS2::position(POSITION_TYPES pty) {
	int32_t t;
	t = set_controlword(pty);
	if(t) {
		Serial.println("ERROR: failed to start profile position mode.");
		kill();
		return;
	}
}
void EPOS2::position() {
	position(RELATIVE_POSITION);
}
void EPOS2::position_stop() {
	int32_t t;
	t = set_controlword(0x010F);
	if(t) {
		Serial.println("ERROR: failed to stop mode profile velocity.");
		kill();
		return;
	}
}
void EPOS2::position(int32_t p, uint32_t v, uint32_t a, uint32_t d, uint32_t qd) {
	int32_t t;
	//disable();
	t = set_quickstop_deceleration(qd);
	if(t) {
		Serial.println("ERROR: failed to set QuickStop Deceleration.");
		kill();
		return;
	}
	position(p, v, a, d);
}
void EPOS2::position(int32_t p, uint32_t v, uint32_t a, uint32_t d) {
	int32_t t;
	//disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(d);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	position(p, v);
}
void EPOS2::position(int32_t p, uint32_t v, uint32_t a) {
	int32_t t;
	//disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(a*3);
	if(t) {
		Serial.println("ERROR: failed to set Quickstop Deceleration.");
		kill();
		return;
	}
	position(p, v);
}
void EPOS2::position(int32_t p, uint32_t v) {
	int32_t t;
	//disable();
	t = set_profile_velocity(v);
	if(t) {
		Serial.println("ERROR: failed to set Profile Velocity.");
		kill();
		return;
	}
	position(p);
}
void EPOS2::position(int32_t p) {
	int32_t t;
	//disable();
	t = set_operation_mode(PROFILE_POSITION_MODE);
	if(t) {
		Serial.println("ERROR: failed to set mode to Profile Position mode.");
		kill();
		return;
	}
	
	//enable();
	t = set_target_position(p);
	if(t) {
		Serial.println("ERROR: failed to set target position.");
		kill();
		return;
	}
}

void EPOS2::run_with_velocity(int32_t v, uint32_t a) {
	int32_t t;
	disable();
	t = set_max_acceleration(a);
	if(t) {
		Serial.println("WARNING : Couldn't set max acceleration");
	}
	run_with_velocity(v);
}
void EPOS2::run_with_velocity(int32_t v) {
	int32_t t;
	t = set_maximal_profile_velocity(v+100);
	if(t) {
		Serial.println("WARNING : Couldn't set maximal profile velocity");
	}

	t = set_operation_mode(VELOCITY_MODE);
	if(t) {
		Serial.println("ERROR: failed to set mode to Velocity mode.");
		kill();
		return;
	}
	disable();
	enable();
	t = set_velocity_mode_setting_value(v);
	if(t) {
		Serial.println("ERROR: failed to set Velocity.");
		kill();
		return;
	}
}

void EPOS2::velocity(int32_t v, uint32_t a, uint32_t d, uint32_t qd, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(d);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(qd);
	if(t) {
		Serial.println("ERROR: failed to set quickstop Deceleration.");
		kill();
		return;
	}
	velocity(v, m);
}
void EPOS2::velocity(int32_t v, uint32_t a, uint32_t d, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(d);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(d*3);
	if(t) {
		Serial.println("ERROR: failed to set quickstop Deceleration.");
		kill();
		return;
	}
	velocity(v, m);
}
void EPOS2::velocity(int32_t v, uint32_t a, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(a*3);
	if(t) {
		Serial.println("ERROR: failed to set Quickstop Deceleration.");
		kill();
		return;
	}
	velocity(v, m);
}
void EPOS2::velocity(int32_t v, MOTION_PROFILES m) {
	int32_t t;
//	disable();
	t = set_operation_mode(PROFILE_VELOCITY_MODE);
	if(t) {
		Serial.println("ERROR: failed to set mode to Profile Position mode.");
		kill();
		return;
	}
	
	t = set_motion_profile_type(m);
	if(t) {
		Serial.println("ERROR: failed to set Motion Profile Type.");
		kill();
		return;
	}
	
//	enable();
	t = set_target_velocity(v);
	if(t) {
		Serial.println("ERROR: failed to set target position.");
		kill();
		return;
	}
}
void EPOS2::velocity() {
	int32_t t;
	t = set_controlword(0x0F);
	if(t) {
		Serial.println("ERROR: failed to start profile position mode.");
		kill();
		return;
	}
}
void EPOS2::velocity_stop() {
	int32_t t;
	t = set_controlword(0x010F);
	if(t) {
		Serial.println("ERROR: failed to stop mode profile velocity.");
		kill();
		return;
	}
}
void EPOS2::velocity(int32_t v, uint32_t a, uint32_t d, uint32_t qd) {
	int32_t t;
	//disable();
	t = set_quickstop_deceleration(qd);
	if(t) {
		Serial.println("ERROR: failed to set QuickStop Deceleration.");
		kill();
		return;
	}
	velocity(v, a, d);
}
void EPOS2::velocity(int32_t v, uint32_t a, uint32_t d) {
	int32_t t;
	//disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(d);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	velocity(v);
}
void EPOS2::velocity(int32_t v, uint32_t a) {
	int32_t t;
	//disable();
	t = set_profile_acceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Acceleration.");
		kill();
		return;
	}
	t = set_profile_deceleration(a);
	if(t) {
		Serial.println("ERROR: failed to set Profile Deceleration.");
		kill();
		return;
	}
	t = set_quickstop_deceleration(a*3);
	if(t) {
		Serial.println("ERROR: failed to set Quickstop Deceleration.");
		kill();
		return;
	}
	velocity(v);
}
void EPOS2::velocity(int32_t v) {
	int32_t t;
	//disable();
	t = set_operation_mode(PROFILE_VELOCITY_MODE);
	if(t) {
		Serial.println("ERROR: failed to set mode to Profile Position mode.");
		kill();
		return;
	}
	
	//enable();
	t = set_target_velocity(v);
	if(t) {
		Serial.println("ERROR: failed to set target position.");
		kill();
		return;
	}
}



