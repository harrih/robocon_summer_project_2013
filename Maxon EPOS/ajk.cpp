// compile : 


# include <iostream>
# include <cstring>
# include <definitions.h>

using namespace std;

class epos2{
	public:
	// Device Name = "EPOS2"
	// Protocol Stack Name = "MAXON SERIAL V2"
	// Interface Name = "USB"
	// Port Name = USB0
	// Baudrate = 1000000
	// Timeout = 500
	int timeout;
	char portName[7];
	unsigned long errorCode;
	void * key;
	unsigned short nodeId;
	
	// constructors
	epos2(int t, char p[7], ) {
		timeout = t;
		strcpy(portName, p);
		errorCode = 0;
	}
	epos2(char p[7]) {
		timeout = 1000000;
		strcpy(portName, p);
		errorCode = 0;
	}
	
	// init and end
	int open() {
		resetErrors();
		this->close();
		
		key = VCS_OpenDevice( "EPOS2", "MAXON SERIAL V2", "USB", portName, &errorCode);
		if(key == 0) {
			cout << "Error, Open device failure. Code: " << errorCode << "\n";
			return 1;
		}
		if( !VCS_SetProtocolStackSettings(key, 1000000, timeout, errorCode) ) {
			cout << "Error, Setting protocol stack settings failed. Code: ") << errorCode << "\n";
			return 1;
		}
		return 0;
	}
	void close() {
		resetErrors();
		if(key != 0)
			VCS_CloseDevice(key, &errorCode);
		VCS_CloseAllDevices(&errorCode);
	}
	int enable() {
		resetErrors();
		
		if(!resetFault()) {
			int isEnabled = 0;
			if( VCS_GetEnableState(key, nodeId, &isEnabled, &errorCode) ) {
				if( !isEnabled && !VCS_SetEnableState(key, nodeId, &errorCode) ) {
					cout << "Error, couldn't enable device. Code: " << errorCode << "\n";
					return 1;
				} else {
					return 0;
				}
			}
		} else {
			return 1;
		}
		return 0;
	}
	int disable() {
		resetErrors();
		
		if(!resetFault()) {
			int isEnabled = 0;
			if( VCS_GetEnableState(key, nodeId, &isEnabled, &errorCode) ) {
				if( isEnabled && !VCS_SetDisableState(key, nodeId, &errorCode) ) {
					cout << "Error, couldn't enable device. Code: " << errorCode << "\n";
					return 1;
				} else {
					return 0;
				}
			}
		} else {
			return 1;
		}
		return 0;
	}
	
	// locomotion
	int stop() {
		resetError();
		
		if( !VCS_HaltPositionMovement(key, nodeId, &errorCode) ) {
			cout << "Error, couldn't stop motor. Code: ", errorCode << "\n";
		}
	}
	int turn(int rot) {
		resetErrors();
		if( VCS_ActivateProfilePositionMode(keyHandle_, nodeId, &errorCode) ) {
			if( !VCS_MoveToPosition(key, nodeId, rot, 0, 1, &errorCode) ) { // 0 - not abs position, 1 - immediately
				cout << "Error, could not turn the motor. Code: " << errorCode << "\n";
				return 1;
			}
		} else {
			cout << "Error, could not activate Position mode. Code: " << errorCode << "\n";
			return 1;
		}
		return 0;
	}
	int turnTo(int rot) {
		resetErrors();
		if( VCS_ActivateProfilePositionMode(keyHandle_, nodeId, &errorCode) ) {
			if( !VCS_MoveToPosition(key, nodeId, rot, 1, 1, &errorCode) ) { // first 1 for - absolute position, second - immediately
				cout << "Error, could not turn the motor. Code: " << errorCode << "\n";
				return 1;
			}
		} else {
			cout << "Error, could not activate Position mode. Code: " << errorCode << "\n";
			return 1;
		}
		return 0;
	}
	int getLocation() {
		if(!key) {
			cout << "Error, Device not enabled." << "\n";
			return 1;
		}
		resetErrors();
		
		long position = 0;
		if( VCS_GetPositionIs(key, nodeId, &position, &errorCode) ) {
			cout << "Position was found to be : " << position << "\n";
		} else {
			cout << "Error, couldn't get position. Code: " << errorCode << "\n";
		}

	}
	
	// misc
	int resetFault() {
		int inFault = 0;
		if( VCS_GetFaultState(key, nodeId, &inFault, &errorCode) ) {
			if( inFault && !VCS_ClearFault(key, nodeId, &errorCode) ) {
				cout << "Error, couldn't remove fault. Code: " << errorCode << "\n";
				return 1;
			}
		} else {
			cout << "Couldn't get fault state. Code: " << errorCode << "\n";
			return 1;
		}
		return 0;
	}
	void resetErrors() {
		errorCode = 0;
	}
}

void main() {

	
}
