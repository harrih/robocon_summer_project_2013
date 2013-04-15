# include "epos2.h"
//# define EPOS2_DEBUG Serial // for debugging into Serial
EPOS2::EPOS2 (HardwareSerial *s, unsigned short n) {
	serial_ = s;
	nodeId = n;
	
	serial_->begin(EPOS2_BAUDRATE);
	serial_flush();
}

void EPOS2::serial_flush () {
	while(serial_->available())
		serial_->read();
}

int EPOS2::wait_for_serial () {
	unsigned long int pmillis = millis();
	while(!serial_->available())
		if(millis()-pmillis > 500)
			return 1;
	return 0;
}

void EPOS2::set_crc (epos2_frame* f) {
	unsigned int crc = get_crc(*f);
	f->crc = crc;
}

unsigned int EPOS2::get_crc (epos2_frame f) {
	//Calculate pDataArray Word by Word
	//Serial.println("function called");
	int data[6] = { (f.opcode<<8) + f.len, f.data[0], f.data[1], f.data[2], f.data[3], 0 };
	data[f.len + 2] = 0; // reset the last array element to 0 (in case random values are there ...
	int numberOfWords = f.len + 3; // the number_of_words (=len+1) + the 0 + opcode/len
	unsigned int opcode = f.opcode;
	
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

int EPOS2::check_crc (epos2_frame f) {
	return (f.crc == get_crc(f));
}

int EPOS2::send (epos2_frame f) {
	int counter_t = 0;
	char c, c2;
	do {
		do {
			if(counter_t++ > 5)
				return 3;
			serial_flush();
			f.data[1] = (nodeId<<8) + (f.data[1] & 0x00ff);
			set_crc(&f);
			# ifdef EPOS2_DEBUG
				EPOS2_DEBUG.println("Sending ... ");
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

int EPOS2::get (epos2_frame* f) {
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

epos2_frame get_frame(unsigned short o, unsigned short l, unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3) {
	epos2_frame f;
	f.opcode = o;
	f.len = l;
	f.data[0] = d0;
	f.data[1] = (d1 & 0x0011); // this is to mask and remove any node ID given
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

uint16_t EPOS2::get_device_version() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2003, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return(f.data[2]);
}

uint32_t EPOS2::get_device_type() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x1000, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

uint16_t EPOS2::get_rs232_timeout() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2005, 0x00, 0, 0));
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

int EPOS2::set_rs232_timeout(uint16_t val_timeout) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x2005, 0x00, val_timeout, 0));
	if(t) return -1;
	return(0);
}

int EPOS2::set_operation_mode(OPERATION_MODES m) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6060, 0x00, m, 0));
	if(t) return -1;
	return(0);
}

int EPOS2::set_max_acc(uint32_t a) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x60C5, 0x00, (uint16_t) a, (uint16_t) a>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_max_acc() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x60C5, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

int EPOS2::set_max_vel(uint32_t a) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607F, 0x00, (uint16_t) a, (uint16_t) a>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_max_vel() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607F, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

int EPOS2::set_min_pos(uint32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607D, 0x01, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_min_pos() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607D, 0x01, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

int EPOS2::set_max_pos(uint32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x607D, 0x02, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_max_pos() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x607D, 0x02, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

int EPOS2::set_max_following_error(uint32_t dp) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x6065, 0x00, (uint16_t) dp, (uint16_t) dp>>16));
	if(t) return -1;
	return(0);
}

uint32_t EPOS2::get_max_following_error() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6065, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

int8_t EPOS2::get_operation_mode() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6061, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((int8_t) f.data[2]);
}

int EPOS2::set_control_word(uint16_t w) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 2, 0x6040, 0x00, (uint16_t) w, 0));
	if(t) return -1;
	return(0);
}

uint16_t EPOS2::get_control_word() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6040, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((uint16_t) f.data[2]);
}

uint16_t EPOS2::get_status_word() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6041, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((uint16_t) f.data[2]);
}

int EPOS2::set_position(int32_t p) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x2062, 0x00, (uint16_t) p, (uint16_t) p>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_position() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x2062, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_actual_position() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6064, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_demand_position() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x6062, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int EPOS2::set_velocity(int32_t v) {
	int t;
	t = send(get_frame(WRITE_OPCODE, 3, 0x206B, 0x00, (uint16_t) v, (uint16_t) v>>16));
	if(t) return -1;
	return(0);
}

int32_t EPOS2::get_velocity() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x206B, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_actual_velocity() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x606C, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}

int32_t EPOS2::get_demand_velocity() {
	int t;
	t = send(get_frame(READ_OPCODE, 1, 0x606B, 0x00, 0, 0));
	if(t) return -1;
	epos2_frame f;
	t = get(&f);
	if(t) return -1;
	return((((int32_t)f.data[3])<<16) + f.data[2]);
}
