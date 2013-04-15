
# ifndef EPOS2_H
# define EPOS2_H
#include <HardwareSerial.h>
#include <Arduino.h>
#include "epos2_const.h"

typedef struct {
	unsigned short 	opcode, // opcode
					len; // note this is sent directly. so it should be length_of_words - 1
	unsigned int 	data[4], // bytes/words to send
					crc; // automatically calculated, no need to set it
} epos2_frame;

typedef struct {
	unsigned short switch_on, // bit 0
					enable_voltage, // bit 1
					quickstop, // bit 2
					enable_operation, // bit 3
					mode_enable, // bit 4
					mode_change_immediate, // bit 5
					mode_absolute, // bit 6
					fault_reset, // bit 7
					mode_halt; // bit 8
} epos2_controlword;

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
	void init(HardwareSerial *s, unsigned short n);
	
	// general communication functions
	unsigned int get_crc(epos2_frame f);
	unsigned int get_crc(int data[], int numberOfWords);
	void serial_flush();
	int wait_for_serial();
	int send(epos2_frame f);
	int send_segments(uint16_t ls[], int len);
	int send_segments_2(uint16_t ls[], int len);
	int get(epos2_frame* f);
	
	// General Device constants
	uint32_t get_device_type(); // 0x1000 - 0x00
	uint16_t get_software_version(); // 0x2003 - 0x01
	uint16_t get_hardware_version(); // 0x2003 - 0x02
	uint32_t get_supported_drive_modes(); // 0x6502 - 0x00
	
	// Device settings
	uint16_t get_rs232_frame_timeout(); // 0x2005 - 0x00
	int set_rs232_frame_timeout(uint16_t ti);
	epos2_controlword get_controlword(); // 0x6040 - 0x00
	uint16_t get_controlword_bits();
	int set_controlword(uint16_t w);
	int set_controlword(epos2_controlword cw);
	epos2_statusword get_statusword(); // 0x6041 - 0x00
	uint16_t get_statusword_bits();
	uint16_t get_motor_type(); // 0x6402 - 0x00
	int set_motor_type(MOTOR_TYPES ty);
	epos2_miscconfig get_misc_config(); // 0x2008 - 0x00
	uint16_t get_misc_config_bits();
	int set_misc_config(epos2_miscconfig mc);
	int set_misc_config(uint16_t c);
	epos2_error_frame get_error_register(); // 0x1001 - 0x00
	uint8_t get_error_register_bits();
	
	// Operation mode settings
		// general
	int8_t get_operation_mode(); // 0x6060 - 0x00
	int set_operation_mode(OPERATION_MODES m);
	uint16_t get_motion_profile_type(); // 0x6086 - 0x00
	int set_motion_profile_type(MOTION_PROFILES ty);
		// acceleration
	uint32_t get_max_acceleration(); // 0x60C5 - 0x00
	int set_max_acceleration(uint32_t a);
	uint32_t get_profile_acceleration(); // 0x6083 - 0x00
	int set_profile_acceleration(uint32_t a);
	uint32_t get_profile_deceleration(); // 0x6084 - 0x00
	int set_profile_deceleration(uint32_t d);
	uint32_t get_quickstop_deceleration(); // 0x6085 - 0x00
	int set_quickstop_deceleration(uint32_t d);
		// velocity
	uint32_t get_maximal_profile_velocity(); // 0x607F - 0x00
	int set_maximal_profile_velocity(uint32_t v);
	int32_t get_velocity_mode_setting_value(); // 0x206B - 0x00
	int set_velocity_mode_setting_value(int32_t v);
	int32_t get_velocity_actual_value(); // 0x606C - 0x00
	int32_t get_velocity_demand_value(); // 0x606B - 0x00
	int32_t get_target_velocity(); // 0x60FF - 0x00
	int set_target_velocity(int32_t v);
	uint32_t get_profile_velocity(); // 0x6081 - 0x00
	int set_profile_velocity(uint32_t p);
	uint32_t get_velocity_window(); // 0x606D - 0x00
	int set_velocity_window(uint32_t w);
	uint16_t get_velocity_window_time(); // 0x606E - 0x00
	int set_velocity_window_time(uint16_t ti);
		// position
	uint32_t get_maximal_following_error(); // 0x6065 - 0x00
	int set_maximal_following_error(uint32_t dp);
	int32_t get_minimal_position_limit(); // 0x607D - 0x01 -- Software position limit
	int set_minimal_position_limit(int32_t p);
	int32_t get_maximal_position_limit(); // 0x607D - 0x02 -- Software position limit
	int set_maximal_position_limit(int32_t p);
	int32_t get_position_mode_setting_value(); // 0x2062 - 0x00
	int set_position_mode_setting_value(int32_t p);
	int32_t get_position_actual_value(); // 0x6064 - 0x00
	int32_t get_position_demand_value(); // 0x6062 - 0x00
	int32_t get_target_position(); // 0x607A - 0x00
	int set_target_position(int32_t p);
	uint32_t get_position_window(); // 0x6067 - 0x00
	int set_position_window(uint32_t w);
	uint16_t get_position_window_time(); // 0x6068 - 0x00
	int set_position_window_time(uint16_t ti);
		// Interpolated position mode
	uint32_t get_ipm_maximum_buffer_size(); // 0x60C4 - 0x01 -- Interpolation Data Configuration
	uint32_t get_ipm_actual_buffer_size(); // 0x60C4 - 0x02 -- Interpolation Data Configuration
	uint16_t get_ipm_buffer_position(); // 0x60C4 - 0x04 -- Interpolation Data Configuration
	int set_ipm_buffer_clear(uint8_t v); // 0x60C4 - 0x05 -- Interpolation Data Configuration
	epos2_ipm_status get_ipm_buffer_status(); // 0x20C4 -- 0x01 -- Interpolation Buffer
	uint16_t get_ipm_buffer_status_bits();
	uint16_t get_ipm_buffer_underflow_warning(); // 0x20C4 -- 0x02 -- Interpolation Buffer
	int set_ipm_buffer_underflow_warning(uint16_t l);
	uint16_t get_ipm_buffer_overflow_warning(); // 0x20C4 -- 0x03 -- Interpolation Buffer
	int set_ipm_buffer_overflow_warning(uint16_t l);
	int add_to_ipm_buffer(epos2_ipm_frame fs[], int len);
	
	// Nice functions
	
};

// EPOS2_frame functions
epos2_frame get_frame(unsigned short o, unsigned short l, unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3);
void print_epos2_frame(epos2_frame f);
# endif //EPOS_H

