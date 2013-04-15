#include <HardwareSerial.h>
#include <Arduino.h>
# ifndef EPOS2_H
# define EPOS2_H

# define EPOS2_BAUDRATE 115200

# define READ_OPCODE 0x10
# define WRITE_OPCODE 0x11

/*# define GET_DEVICE_TYPE			0x10, 1, 0x1000, 0x00, 0, 0
# define GET_ERROR_REGISTER			0x10, 1, 0x1001, 0x00, 0, 0 
# define GET_DEVICE_NAME			0x10, 1, 0x1008, 0x00, 0, 0 
# define GET_NODE_ID				0x10, 1, 0x2000, 0x00, 0, 0 
# define SET_NODE_ID				0x11, 1, 0x2000, 0x00, 0, 0 
# define GET_RS232_BITRATE			0x10, 1, 0x2002, 0x00, 0, 0 
# define SET_RS232_BITRATE			0x11, 1, 0x2002, 0x00, 0, 0 

# define GET_DEVICE_VERSION			0x10, 1, 0x2003, 0x01, 0, 0
*/

typedef enum {
	INTERPOLATED_POSITION_MODE=7,
	HOMING_MODE=6,
	PROFILE_VELOCITY_MODE=3,
	PROFILE_POSITION_MODE=1,
	POSITION_MODE=-1,
	VELOCITY_MODE=-2,
	CURRENT_MODE=-3,
	DIAGNOSTIC_MODE=-4,
	MASTER_ENCODER_MODE=-5,
	DIRECTION_MODE=-6
}OPERATION_MODES;

typedef enum {
	DC_MOTOR=1,
	SIN_EC_MOTOR=10,
	TRAPEZOID_EC_MOTOR=11,
	MANUFACTURE_EC_MOTOR=65535
}MOTOR_TYPES;

typedef enum {
	TRAPEZOIDAL_PROFILE=0,
	SINUSOIDAL_PROFILE=1
}MOTION_PROFILES;

typedef struct {
	unsigned short 	opcode, // opcode
					len; // note this is sent directly. so it should be length_of_words - 1
	unsigned int 	data[4], // bytes/words to send
					crc; // automatically calculated, no need to set it
} epos2_frame;

class EPOS2 {
protected:
	HardwareSerial *serial_; // the serial interface to use.
	unsigned int nodeId;
	void set_crc(epos2_frame* f);
	unsigned int get_crc(epos2_frame f);
	int check_crc(epos2_frame f);
	void serial_flush();
	int wait_for_serial();
	
	
private:
	
public:
	EPOS2(HardwareSerial *s, unsigned short n);
	int send(epos2_frame f);
	int get(epos2_frame* f);
	
	uint32_t get_device_type();
	uint16_t get_device_version();
	uint16_t get_rs232_timeout();
	uint32_t get_supported_drive_modes();
	int8_t get_operation_mode();
	uint32_t get_max_acc();
	uint32_t get_max_vel();
	int32_t get_max_pos();
	int32_t get_min_pos();
	uint32_t get_max_following_error();
	uint16_t get_control_word();
	uint16_t get_status_word();
	int32_t get_position_setting();
	int32_t get_actual_position();
	int32_t get_demand_position();
	int32_t get_velocity_setting();
	int32_t get_profile_target_position();
	int32_t get_profile_target_velocity();
	uint32_t get_profile_velocity();
	uint32_t get_profile_acceleration();
	uint32_t get_profile_deceleration();
	uint16_t get_profile_motion();
	uint32_t get_profile_quickstop_deceleration();
	int32_t get_actual_velocity();
	int32_t get_demand_velocity();
	uint16_t get_misc_config();
	uint16_t get_motor_type();
	uint32_t get_max_ipm_buffer();
	uint32_t get_actual_ipm_buffer();
	uint16_t get_ipm_buffer_position();
	uint16_t get_ipm_buffer_status();
	uint32_t get_position_window();
	uint16_t get_position_window_time();
	
	int set_rs232_timeout(uint16_t val_timeout);
	int set_operation_mode(OPERATION_MODES m);
	int set_max_acc(uint32_t a);
	int set_max_vel(uint32_t a);
	int set_max_pos(uint32_t p);
	int set_min_pos(uint32_t p);
	int set_max_following_error(uint32_t dp);
	int set_control_word(uint16_t w);
	int set_position_setting(int32_t p);
	int set_velocity_setting(int32_t p);
	int set_profile_target_position(int32_t p);
	int set_profile_target_velocity(int32_t v);
	int set_profile_velocity(uint32_t p);
	int set_profile_acceleration(uint32_t p);
	int set_profile_deceleration(uint32_t p);
	int set_profile_motion(MOTION_PROFILES ty);
	int set_profile_quickstop_deceleration(uint32_t d);
	int set_misc_config(uint16_t p);
	int set_motor_type(MOTOR_TYPES ty);
	int set_ipm_buffer(uint8_t v);
	int set_position_window(uint32_t w);
	int set_position_window_time(uint16_t ti);
	int add_to_ipm_buffer(uint32_t p, uint32_t v, uint8_t ti);
	
	
};

epos2_frame get_frame(unsigned short o, unsigned short l, unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3);
void print_epos2_frame(epos2_frame f);
# endif //EPOS_H

