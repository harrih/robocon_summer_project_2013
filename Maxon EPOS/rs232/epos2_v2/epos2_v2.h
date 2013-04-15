# ifndef EPOS2_V2_H
# define EPOS2_V2_H


#include <HardwareSerial.h>
#include <Arduino.h>
#include "epos2_const_v2.h"
#include "epos2_objects_v2.h"

typedef struct {
	uint8_t 	opcode, // opcode
				len; // note this is sent directly. so it should be length_of_words - 1
	uint16_t 	data[4], // bytes/words to send
				crc; // automatically calculated, no need to set it
}epos2_frame;

/*typedef struct {
	unsigned short switch_on, // bit 0
					enable_voltage, // bit 1
					quickstop, // bit 2
					enable_operation, // bit 3
					mode_enable, // bit 4
					mode_change_immediate, // bit 5
					mode_absolute, // bit 6
					fault_reset, // bit 7
					mode_halt; // bit 8
}epos2_controlword;

typedef struct {
	unsigned short ready_to_switch_on, // bit 0
					switched_on, // bit 1
					operation_enable, // bit 2 
					fault, // bit 3
					voltage_enabled, // bit 4
					quickstop, // bit 5
					switch_on_disable, // bit 6
					warning, // bit 7
					offset_current_measured, // bit 8
					remote, // bit 9
					mode_target_reached, // bit 10
					internal_limit_active, // bit 11
					mode_active, // bit 12
					mode_error, // bit 13
					refresh_cycle_of_power_stage, // bit 14
					positon_referenced_to_home_position; // bit 15
} epos2_statusword;

typedef struct {
	unsigned short sensor_supervision_by_software, 
					sensor_supervision_by_hardware,
					measure_dc_motor_resistance,
					measure_motor_speed_with_main_encoder,
					measure_motor_speed_with_auxiliary_encoder,
					polarity_bit;
} epos2_miscconfig;

typedef struct {
	unsigned short underflow_warning, // bit 0
					overflow_warning, // bit 1
					velocity_warning, // bit 2
					acceleration_warning, // bit 3
					underflow_error, // bit 8
					overflow_error, // bit 9
					velocity_error, // bit 10
					acceleration_error, // bit 11
					buffer_enabled, // bit 14
					ip_mode_active; // bit 15
} epos2_ipm_status;

typedef struct {
	unsigned short generic, // bit 0
					current, // bit 1 
					voltage, // bit 2
					temperature, // bit 3
					communication, // bit 4
					device_profile_specific, // bit 5
					motion_error; // bit 7
} epos2_error_frame;

typedef struct {
	int32_t p,
			v;
	uint8_t t;
} epos2_ipm_frame;
*/

// Class definition

class EPOS2 {
public:
	// internal variables
	HardwareSerial *serial_; // the serial interface to use.
	unsigned int nodeId;
	
	// epos2_frame functions
	void set_crc(epos2_frame* f);
	int check_crc(epos2_frame f);
	
	// contructor destructor
	EPOS2();
	EPOS2(HardwareSerial *s, unsigned short n);
	EPOS2(HardwareSerial *s, unsigned short n, EPOS2_MOTOR_ROTATIONS d);
	void init(HardwareSerial *s, unsigned short n);
	void init(HardwareSerial *s, unsigned short n, EPOS2_MOTOR_ROTATIONS d);
	void enable();
	void disable();
	
	// general communication functions
	unsigned int get_crc(epos2_frame f);
	unsigned int get_crc(int data[], int numberOfWords);
	void serial_flush();
	int wait_for_serial();
	int set(epos2_frame f);
	uint32_t set(int len, uint16_t index, uint8_t subindex, uint32_t val);
	int get(epos2_frame* f);
	uint32_t get(int len, uint16_t index, uint8_t subindex, uint32_t* err);
	
	// Simultaneous Communication functions
	uint32_t set_set(epos2_frame f1, EPOS2 e2, epos2_frame f2);
	uint32_t set_set(int len1, uint16_t index1, uint8_t subindex1, uint32_t val1, EPOS2 e2, int len2, uint16_t index2, uint8_t subindex2, uint32_t val2);
	uint32_t get_get(epos2_frame f1, EPOS2 e2, epos2_frame f2);
	uint32_t get_get(int len1, uint16_t index1, uint8_t subindex1, uint32_t err1, EPOS2 e2, int len2, uint16_t index2, uint8_t subindex2, uint32_t err2);

};

// EPOS2_frame functions
epos2_frame get_frame(uint8_t o, uint8_t l, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3);
void print_epos2_frame(epos2_frame f);

# endif //EPOS_H


