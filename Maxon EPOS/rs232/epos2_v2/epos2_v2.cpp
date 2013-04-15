# include "epos2_v2.h"
//# define EPOS2_DEBUG Serial // for debugging into Serial
# define output_print(x) Serial.print(x)
# define output_println(x) Serial.println(x)
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
}
void EPOS2::init(HardwareSerial *s, unsigned short n, EPOS2_MOTOR_ROTATIONS d) {
  init(s, n);
  
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

int EPOS2::set (epos2_frame f) { // Sends the frame to epos, and handles complete comm for it
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
uint32_t EPOS2::set(int len, uint16_t index, uint8_t subindex, uint32_t val) {
	int t;
	len = ((len+1)/2) + 1;
	t = set(get_frame(WRITE_OPCODE, len, index, subindex, (uint16_t) val, (uint16_t) (val>>16)));
	if(t) {
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("<<<<< ERROR >>>> Communication error : ");
			EPOS2_DEBUG.println(t);
		#endif
		return -1;
	}
/*	epos2_frame f;
	t = get(&f);
	if(t) {
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("<<<<< ERROR >>>> Code : ");
			EPOS2_DEBUG.println(t);
			EPOS2_DEBUG.print("While getting from index : ");
			EPOS2_DEBUG.print(index);
			EPOS2_DEBUG.print("    subindex : ");
			EPOS2_DEBUG.println(subindex);
		#endif
		return -1;
	}
	return((((uint32_t)f.data[1])<<16) + f.data[0]);
*/	return 0;
}

int EPOS2::get (epos2_frame* f) { // Gets a frame from epos, and handles complete comm for it
	int count_t = 0;
	uint8_t temps;
	uint16_t temp;
	uint32_t templ;
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
							// CHECK FOR ERROR !
							templ = ((uint32_t)f->data[1]<<16) + f->data[0];
							if(templ)
								return templ;
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
uint32_t EPOS2::get(int len, uint16_t index, uint8_t subindex, uint32_t* err) {
	int t;
	t = set(get_frame(READ_OPCODE, 1, index, subindex, 0, 0));
	if(t) {
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("<<<<< ERROR >>>> Communication error : ");
			EPOS2_DEBUG.println(t);
		#endif
		return -1;
	}
	epos2_frame f;
	t = get(&f);
	if(t) {
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("<<<<< ERROR >>>> Code : ");
			EPOS2_DEBUG.println(t);
			EPOS2_DEBUG.print("While getting from index : ");
			EPOS2_DEBUG.print(index);
			EPOS2_DEBUG.print("    subindex : ");
			EPOS2_DEBUG.println(subindex);
		#endif
		*err = t;
		return -1;
	} else {
		err = 0;
	}
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
}

// --------------------------------------------------------------------
// Simultaneous communication functions
//---------------------------------------------------------------------

uint32_t EPOS2::set_set(epos2_frame f1, EPOS2 e2, epos2_frame f2) {
	EPOS2 e1 = *this; // to make it easier to unserstand
	int counter_t = 0;
	char c1, c2;
	f1.data[1] = (e1.nodeId<<8) + (f1.data[1] & 0x00ff);
	f2.data[1] = (e2.nodeId<<8) + (f2.data[1] & 0x00ff);

	e2.set_crc(&f2);
	e1.set_crc(&f1);
	do {
		do {
			do {
				if(counter_t++ > 5)
					return 3;
				do {  
					if(counter_t++ > 5)
						return 3;
					e1.serial_flush();
					e1.serial_->write(f1.opcode);
				} while(e1.wait_for_serial());
				e2.serial_flush();
				e2.serial_->write(f2.opcode);
			} while(e2.wait_for_serial());
			c1 = e1.serial_->read();
			c2 = e2.serial_->read();
			if( c1 == 'O' && c2 == 'O') {
				e1.serial_->write(f1.len);
				e2.serial_->write(f2.len);
				int i = 0;
				for( i = 0; i <= f1.len; i++ ) {
					e1.serial_->write(f1.data[i]);
					e1.serial_->write(f1.data[i]>>8);
					e2.serial_->write(f2.data[i]);
					e2.serial_->write(f2.data[i]>>8);
				}
				e2.serial_->write(f2.crc);
				e1.serial_->write(f1.crc);
				e2.serial_flush(); // flush it so after sending don't get garbage values
				e2.serial_->write(f2.crc>>8);
			}
		} while(e2.wait_for_serial());
		if( c1 == 'O' || c2 == 'O' ) {
			c2 = e2.serial_->read();
			e1.serial_flush(); // flush it so after sending don't get garbage values
			e1.serial_->write(f1.crc>>8);
		}
	} while(e1.wait_for_serial());
	if(c1 == 'O' && c2 == 'O') {
		c1 = e1.serial_->read();
		if( c1 == 'O' && c2 == 'O' ) {
			return 0;
		} else if( c1 == 'F' || c2 == 'F' ) {
			return 2;
		} else {
			return -2;
		}   
		return 0;
	} else if( c1 == 'F' || c2 == 'F' ) {
		return 1;
	} else {
		return -1;
	}
}
uint32_t EPOS2::set_set(int len1, uint16_t index1, uint8_t subindex1, uint32_t val1, EPOS2 e2, int len2, uint16_t index2, uint8_t subindex2, uint32_t val2) {
	int t;
	len1 = ((len1+1)/2) + 1;
	len2 = ((len2+1)/2) + 1;
	t = set_set(get_frame(WRITE_OPCODE, len1, index1, subindex1, (uint16_t) val1, (uint16_t) (val1>>16)),
				e2,
				get_frame(WRITE_OPCODE, len2, index2, subindex2, (uint16_t) val2, (uint16_t) (val2>>16)));
	if(t) {
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("<<<<< ERROR >>>> Communication error : ");
			EPOS2_DEBUG.println(t);
		#endif
		return -1;
	}
/*	epos2_frame f;
	t = get(&f);
	if(t) {
		# ifdef EPOS2_DEBUG
			EPOS2_DEBUG.print("<<<<< ERROR >>>> Code : ");
			EPOS2_DEBUG.println(t);
			EPOS2_DEBUG.print("While getting from index : ");
			EPOS2_DEBUG.print(index);
			EPOS2_DEBUG.print("    subindex : ");
			EPOS2_DEBUG.println(subindex);
		#endif
		return -1;
	}
	return((((uint32_t)f.data[3])<<16) + f.data[2]);
*/	return 0;
}

uint32_t EPOS2::get_get(epos2_frame f1, EPOS2 e2, epos2_frame f2) {
	
}
uint32_t EPOS2::get_get(int len1, uint16_t index1, uint8_t subindex1, uint32_t err1, EPOS2 e2, int len2, uint16_t index2, uint8_t subindex2, uint32_t err2) {
	
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

