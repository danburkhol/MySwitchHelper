
#include "MySwitchHelper.h"

MySwitchHelper::MySwitchHelper() {
	//Empty Constructor
}

MySwitchHelper::MySwitchHelper(int TX_PIN, int RX_PIN) {
	enableTransmit(TX_PIN);
	setRepeatTransmit(10);
	enableReceive(RX_PIN);
}

MySwitchHelper::MySwitchHelper(int TX_PIN, int RX_PIN, SWITCH _mySwitches, int _size) {
	enableTransmit(TX_PIN);
	setRepeatTransmit(10);
	enableReceive(RX_PIN);

	//NUMBER_OF_SWITCHES = _size;
	//switches = _mySwitches;
}

MySwitchHelper::MySwitchHelper(int TX_PIN, int RX_PIN, int numTx) {
	enableTransmit(TX_PIN);
	setRepeatTransmit(numTx);
	enableReceive(RX_PIN);
}

MySwitchHelper::MySwitchHelper(int TX_PIN, int RX_PIN, int numTx, SWITCH _mySwitches, int _size) {
	enableTransmit(TX_PIN);
	setRepeatTransmit(numTx);
	enableReceive(RX_PIN);
	//NUMBER_OF_SWITCHES = _size;
	//switches = _mySwitches;
}


void MySwitchHelper::setSwitches(SWITCH *_mySwitches) {
	switches = _mySwitches;
}

SWITCH* MySwitchHelper::getSwitches() {
	return switches;
}


void MySwitchHelper::printSignal() {
	unsigned long value = getReceivedValue();

	if (value == 0) {
		Serial.print("Unknown encoding");
	} else {
	    Serial.print("Received ");
	    Serial.print( getReceivedValue() );
	    Serial.print(" / ");
	    Serial.print( getReceivedBitlength() );
	    Serial.print("bit ");
	    Serial.print(" Recev Delay: ");
	    Serial.print( getReceivedDelay());
	    Serial.print("  Protocol: ");
	    Serial.println( getReceivedProtocol() );
	}
}

void MySwitchHelper::processIncomingSignal() {

	printSignal();

	//Reset RX and prepare for next signal
	resetAvailable();
}

matched_switch MySwitchHelper::matchSwitch() {
	matched_switch m;	

	printSignal();

  	Serial.println("----- MySwitch Matching -----");

  	for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    //If the value matches either on code or off code
    //Update the controller with the new state

  		if ( switches[i].onCode == getReceivedValue() ) {
  			Serial.println("Match! Returning match_switch");

  			m.childId = i;
  			m.newState = 1;

  			resetAvailable();
  			return m;

  		}else if ( switches[i].offCode == getReceivedValue() ) {
  			Serial.println("Match! Returning match_switch");

  			m.childId = i;
  			m.newState = 0;

  			resetAvailable();
  			return m;

  		}


  	}


	m.childId =  255;
	m.newState = 255;
    Serial.println("NO MATCH");
    resetAvailable();
    return m;
}

void MySwitchHelper::transmitSwitchCmd(int childId, int state) {
	Serial.println("Transmitting to: ");
	Serial.println(switches[childId].name);
	Serial.println("New State: ");
	Serial.println(state);

	if (state == 1) {
		send(switches[childId].onCode, switches[childId].bitlength);

	}else if (state == 0) {
		send(switches[childId].offCode, switches[childId].bitlength);

	}

}

















