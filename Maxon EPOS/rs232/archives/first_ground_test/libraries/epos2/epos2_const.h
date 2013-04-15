# ifndef EPOS2_CONST_H
# define EPOS2_CONST_H


# define EPOS2_BAUDRATE 115200

# define READ_OPCODE 0x10
# define INIT_SEGMENT_READ_OPCODE 0x12
# define SEGMENT_READ_OPCODE 0x14
# define WRITE_OPCODE 0x11
# define INIT_SEGMENT_WRITE_OPCODE 0x13
# define SEGMENT_WRITE_OPCODE 0x15

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

# endif
