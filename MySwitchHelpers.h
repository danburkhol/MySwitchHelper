#include <RCSwitch.h>
#define NUMBER_OF_SWITCHES 2  //Total number of switches

struct SWITCH {
	String name;
	unsigned long onCode;
	unsigned long offCode;
	int bitlength;

};

struct matched_switch {
	int childId;
	int newState;
};

class MySwitchHelper : public RCSwitch {

	//int NUMBER_OF_SWITCHES;
	SWITCH switches[NUMBER_OF_SWITCHES] = {
	  //{name, on_code, off_code, bitlength}
	  {"Front Patio", 5363020, 5363008, 24}, {"Living Room Roto 2", 5592407, 5592404, 24}

	};
	

	public:	

		MySwitchHelper();
		MySwitchHelper(int TX_PIN, int RX_PIN);
		MySwitchHelper(int TX_PIN, int RX_PIN, SWITCH _mySwitches, int _size);
		MySwitchHelper(int TX_PIN, int RX_PIN, int numTx);
		MySwitchHelper(int TX_PIN, int RX_PIN, int numTx, SWITCH _mySwitches, int _size);

		void processIncomingSignal();
		matched_switch matchSwitch();
		void printSignal();
		void setSwitches(SWITCH _mySwitches, int _size);

		void transmitSwitchCmd(int childId, int state);

		SWITCH* getSwitches();
};