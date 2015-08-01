
//{name, on_code, off_code, bitlength}

#include <RCSwitch.h>
#include "MySwitch.h"


MySwitch::MySwitch() {


}
MySwitch::MySwitch(String _name, unsigned long _on_code, unsigned long _off_code, int _bitlength) {
	name = _name;
	on_code = _on_code;
	off_code = _off_code;
	bitlength = _bitlength;
}

MySwitch::MySwitch(String *_name, unsigned long *_on_code, unsigned long *_off_code, int *_bitlength, RCSwitch *_rcSwitch) {
	name = (*_name);
	on_code = (*_on_code);
	off_code = (*_off_code);
	bitlength = (*_bitlength);
	rcSwitch = (*_rcSwitch);
}


MySwitch::MySwitch(String _name, unsigned long _on_code, unsigned long _off_code, int _bitlength, RCSwitch *_rcSwitch) {
	name = _name;
	on_code = _on_code;
	off_code = _off_code;
	bitlength = _bitlength;
	rcSwitch = (*_rcSwitch);
}

void MySwitch::printSwitch() {
	Serial.println(name);
	Serial.println(on_code);
	Serial.println(off_code);
	Serial.println(bitlength);
}




void MySwitch::transmit24bit() {
	rcSwitch.send(off_code, 24);
	Serial.println("Transmit24bit no pointer");

}

void MySwitch::transmit24bit(RCSwitch *rcSwitch) {
	(*rcSwitch).send(off_code, 24);
  	Serial.println("Tx Complete");
}


void MySwitch::send(bool state) {
	if (state) {
		on();
	}else {
		off();
	}
	
}

void MySwitch::send(int state) {
	if (state == 1) {
		on();
	}else {
		off();
	}
	
}


void MySwitch::on() {
	transmit24(on_code);
}

void MySwitch::off() {
	transmit24(off_code);
}

void MySwitch::transmit24(unsigned long code) {
	rcSwitch.send(code, 24);
}


bool MySwitch::match(unsigned long value) {
	//Given a signal, see if it matches any known codes of this switch
	//Serial.println("MATCHING SIGNAL");

	if (on_code == value || off_code == value){
		return true;
	} else {
		return false;
	}

}

bool MySwitch::matchSignal(unsigned long value) {
	
	if (on_code == value) {

		return 1;
	}else if (off_code == value){

		return 0;
	}

}



//Getters
String MySwitch::getName() {
	return name;
}

unsigned long MySwitch::getOnCode() {
	return on_code;
}

unsigned long MySwitch::getOffCode() {
	return off_code;
}

int MySwitch::getBitlength() {
	return bitlength;
}