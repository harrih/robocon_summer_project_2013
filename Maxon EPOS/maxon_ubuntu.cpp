/* TODO:
 * When moving, it doesnt work when i try it first time. If i use the epos's example and then try, only then it works ! ?
 * There must be some command Im not putting whioch is done by it ???
 *
 * The vel and current is non zero even when not moving ? initially its all zero. later after using epos's own example to move motor,
 * it gives non zero values ??
 *
 */
//#include "main_dialog.h"
//#include "settings_dialog.h"
//#include <QtGui>
//#include <QApplication>
#include "Definitions.h"
#include <stdio.h>
#include <iostream>
#define TRUE 1
#define FALSE 1

//#include "epos_properties.h"

using namespace std;

void * keyHandle = 0;
int nodeId = 1;

// basics

int setup_epos2() {
// open device
    unsigned long ErrorCode = 0;
    printf("Setting up EPOS2 ... ");
	keyHandle = VCS_OpenDevice(	"EPOS2",
									"MAXON SERIAL V2",
									"USB",
									"USB0",
									&ErrorCode);

	if( keyHandle == 0 )
	{
		printf("\ndevice failed to open!\n");
		return 1;
	}
	printf("device opened ... ");

// set basic settings for comm
	if( !VCS_SetProtocolStackSettings(keyHandle,
			1000000, // baud rate
			60000, // timeout
			&ErrorCode) )
	{
		printf("\nstack setting failed !\n");
		return 2;
	}
	printf("stack set ... ");
	printf("DONE \n");
	return 0;

}

int close_epos2() {
// close device first
    unsigned long ErrorCode = 0;
	printf("Closing connection to device ... ");
	if(keyHandle != 0)
        VCS_CloseDevice(keyHandle, &ErrorCode);

    VCS_CloseAllDevices(&ErrorCode);
	printf("DONE \n");
	return 0;

}

int enable_epos2() {
//  enable controller
	int IsInFault = FALSE;
    unsigned long ErrorCode = 0;
	printf("Enabling the EPOS2 controller ... ");
	if( VCS_GetFaultState(0, nodeId, &IsInFault, &ErrorCode) ) // 1 = nodeId
	{
		printf("Fault found ... ");
		if( IsInFault && !VCS_ClearFault(keyHandle, nodeId, &ErrorCode) )
		{
			printf("\nClearing fault failed !\n");
			return 1;
		}
		printf("Cleared ... ");
		int IsEnabled = FALSE;
		if( VCS_GetEnableState(keyHandle, nodeId, &IsEnabled, &ErrorCode) )
		{
			if( !IsEnabled && !VCS_SetEnableState(keyHandle, nodeId, &ErrorCode) )
			{
				printf("\nCould not set enabled state !\n");
				return 2;
			}

			printf("enabled ... \n");
		}
	}

	printf("DONE \n");
	return 0;
}

int disable_epos2() {
//  disable controller
    unsigned long ErrorCode = 0;
	int IsInFault = FALSE;
	printf("Disabling device ... ");
	if( VCS_GetFaultState(0, nodeId, &IsInFault, &ErrorCode) ) // 1 = nodeId
	{
		printf("fault found ... ");
		if( IsInFault && !VCS_ClearFault(keyHandle, nodeId, &ErrorCode) )
		{
			printf("\nClearing fault failed during disabling !\n");
			return 1;
		}
		printf("cleared ... ");

		int IsEnabled = FALSE;
		if( VCS_GetEnableState(keyHandle, nodeId, &IsEnabled, &ErrorCode) )
		{
			if( IsEnabled && !VCS_SetDisableState(keyHandle, nodeId, &ErrorCode) )
			{
				printf("\nCould not set disabled state !\n");
				return 2;
			}
			printf("disabled ... ");
		}
	}
	printf("DONE \n");
	return 0;
}

// get info from epos

int show_position_epos2() {
// get initial positions
	long int PositionIs = 0;
    unsigned long ErrorCode = 0;

	if( VCS_GetPositionIs(keyHandle,
	    	nodeId, // node id
			&PositionIs,
			&ErrorCode) )
		printf("Encoder value = %ld \n", PositionIs);

	return 0;
}

int show_current_epos2() {
// get initial positions
	short CurrentIs = 0;
    unsigned long ErrorCode = 0;

	if( VCS_GetCurrentIs(keyHandle,
	    	nodeId, // node id
			&CurrentIs,
			&ErrorCode) )
		printf("Current = %d \n", CurrentIs);

	return 0;
}

int show_velocity_epos2() {
// get initial positions
	short int VelocityIs = 0;
    unsigned long ErrorCode = 0;

	if( VCS_GetCurrentIs(keyHandle,
	    	nodeId, // node id
			&VelocityIs,
			&ErrorCode) )
		printf("Velocity = %d \n", VelocityIs);

	return 0;
}

// control epos

int move_abs_with_epos2(long int TargetPosition) {
    unsigned long ErrorCode = 0;

	printf("Moving absolutely by ... %ld ... ", TargetPosition);
    if( VCS_ActivateProfilePositionMode(keyHandle, nodeId, &ErrorCode) ) // 1 is for node id
    {
		printf("Profile set ... ");
    	int Absolute = TRUE;
    	int Immediately = TRUE;

    	if( !VCS_MoveToPosition(keyHandle, nodeId, TargetPosition, Absolute, Immediately, &ErrorCode) ) // 1 is for nodeID
    	{
    		printf("\nMove to position failed!, error code=0x%lu\n", ErrorCode);
			return 1;
    	}
    	printf("reached ... ");
    }
    else
    {
		printf("\n Couldnt  activate profile \n");
    	return 2;
    }
	printf("DONE \n");
	return 0;
}

int move_rel_with_epos2(long int TargetPosition) {
    unsigned long int ErrorCode = 0;

	printf("Moving relatively by ... %ld ... ", TargetPosition);

    if( VCS_ActivateProfilePositionMode(keyHandle, nodeId, &ErrorCode) ) // 1 is for node id
    {
		printf("Profile set ...", ErrorCode);
    	int Absolute = FALSE;
    	int Immediately = TRUE;

    	if( !VCS_MoveToPosition(keyHandle, nodeId, TargetPosition, Absolute, Immediately, &ErrorCode) ) // 1 is for nodeID
    	{
    		printf("\nMove to position failed!, error code=0x%lu\n", ErrorCode);
			return 1;
    	}
    	printf("reached ... %lu", ErrorCode);
    }
    else
    {
		printf("\nCouldnt activate profile \n");
    	return 2;
    }
	printf("DONE \n");
	return 0;
}

int stop_with_epos2() {
	unsigned long ErrorCode = 0;

	if( !VCS_HaltPositionMovement(keyHandle, nodeId, &ErrorCode) ) // 1 is for nodeID
	{
		printf("Halt position movement failed!, error code=0x%lu\n", ErrorCode);

	}
}




















int main(int argc, char *argv[]) {

	//unsigned long	timeout_ = 60000;
	//unsigned long	baudrate_ = 1000000;
	//QString deviceName_ = "EPOS2";
	//QString protocolStackName_ = "MAXON SERIAL V2";
	//QString interfaceName_ = "USB";
	//QString portName_ = "USB0";

	close_epos2();
	setup_epos2();

	enable_epos2();
	char c = 'a';
	while(c != 'q') {
		printf("\ne - Enable device\n");
		printf("d - Disable device\n");
		printf("p - Show Encoder value (position)\n");
		printf("v - Show velocity drawn\n");
		printf("c - Show Current being drawn\n");
		printf("r - Move motor relatively to start point\n");
		printf("a - Move motor absolutely\n");
		printf("i - Get info\n");
		printf("q - Quit\n");
		printf(">> ");
		cin >> c; // scanf was gen cupping
		if(c == 'e') {
			printf("\n");
			enable_epos2();
		} else if(c == 'd') {
			printf("\n");
			disable_epos2();
		} else if(c == 'p') {
			printf("\n");
			show_position_epos2();
		} else if(c == 'c') {
			printf("\n");
			show_current_epos2();
		} else if(c == 'v') {
			printf("\n");
			show_velocity_epos2();
		} else if(c == 'r') {
			printf("\n");
			long int val = 0;
			printf(" Input encoder counts for target : ");
			scanf("%ld", &val);
			move_rel_with_epos2(val);
		} else if(c == 'a') {
			printf("\n");
			long int val = 0;
			printf(" Input encoder counts for target : ");
			scanf("%ld", &val);
			move_abs_with_epos2(val);
		} else if ( c == 'i') {
			show_position_epos2();
			show_current_epos2();
			show_velocity_epos2();
		} if( c == 'q') {
			break;
		}
	//	fflush(stdin);
	}
	//show_pos_epos2();

	disable_epos2();
}
