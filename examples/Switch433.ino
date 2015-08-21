/*  Arduino Sketch for a MySensors-compatible 433 MHz Transmitter

    Sketch I used to control two GE remote outlets

    Daniel Burkholder 
    Version 1.0
*/

#include <RCSwitch.h>   //433 Transmitter
#include <EEPROM.h>
#include "MySwitchHelper.h"

#include <MySigningNone.h>
#include <MyTransportNRF24.h>
#include <MyTransportRFM69.h>
#include <MyHwATMega328.h>
#include <MySensor.h>
#include <SPI.h>


#define TX_PIN 8        //Transmitter PIN#8
#define RX_PIN 1        // Interrupt 0 => Pin #2
                        // Interrupt 1 => Pin #3


#define NUMBER_OF_SWITCHES 2  //Total number of switches


//----- MySensors -----
// NRFRF24L01 radio driver (set low transmit power by default) 
MyTransportNRF24 radio(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);  
//MyTransportRFM69 radio;
// Message signing driver (none default)
//MySigningNone signer;
// Select AtMega328 hardware profile
MyHwATMega328 hw;
// Construct MySensors library
MySensor gw(radio, hw);
//----- MySensors -----


SWITCH my_switches[NUMBER_OF_SWITCHES] = {
  //{name, on_code, off_code, bitlength}
  {"Front Patio", 5363020, 5363008, 24}, {"Living Room Roto 2", 5592407, 5592404, 24}

};

// RCSwitch rcSwitch = RCSwitch();
MySwitchHelper rcSwitch = MySwitchHelper(TX_PIN, RX_PIN, 10);



void setup()  {  
  Serial.begin(115200);

  //Setup LEDs
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);


  ledWhiteOn();
  //Setup MySensor and assign func for incoming messages from the Controller
  gw.begin(incomingMessage, AUTO, true);
  gw.sendSketchInfo("433Transceiver", "1.0");
  

  // Register all switches with controller (they will be created as child devices)
  ledRedOn();
  rcSwitch.setSwitches(my_switches);
  presentSwitches();


  ledOff();
  Serial.println("----- Setup Complete -----");

}



void loop() {
  //Process any incoming message from Controller
  gw.process();


  //If a 433 MHz transmission is received
  if ( rcSwitch.available() ) {
    matchSwitch();

  }
  

  ledOff();
} 


//
void presentSwitches() {
  //Present the array of switches to the Controller
  Serial.println("----- Begin Presentation -----");


  SWITCH *a = rcSwitch.getSwitches();

  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    Serial.println("Presenting: ");
    Serial.println( a[i].name );
    
    gw.present(i, S_LIGHT);
  }


  Serial.println("----- End Presentation -----");
}



//
void matchSwitch() {
  matched_switch m = rcSwitch.matchSwitch();

  //m.childId = 255 = no match to known switches
  if (m.childId != 255) {
    //Match found
    updateController(m.childId, m.newState, V_LIGHT);

  }

}


//
void updateController(int childSensorId, int newState, int TYPE) {
  ledGreenOn();

  MyMessage updateMsg(childSensorId, TYPE);
  updateMsg.set(newState);

  gw.send(updateMsg);

}


//====================


//Incoming Message from the Controller
void incomingMessage(const MyMessage &message) {
  Serial.println("----- MSG FROM CONTROLLER -----");

  if (message.type==V_LIGHT) {
    ledGreenOn();

    // Transmit 433 Switch command to specified switch
    rcSwitch.transmitSwitchCmd(message.sensor, message.getBool());

  } 

}




//===== LED funcs

void ledGreenOn() {
  digitalWrite(LED_GREEN, HIGH);
}

void ledGreenOff() {
  digitalWrite(LED_GREEN, LOW);
}

void ledWhiteOn() {
  digitalWrite(LED_WHITE, HIGH);
}

void ledWhiteOff() {
  digitalWrite(LED_WHITE, LOW);
}

void ledRedOn() {
  digitalWrite(LED_RED, HIGH);
}

void ledRedOff() {
  digitalWrite(LED_RED, LOW);
}

void ledOff() {
  digitalWrite(LED_WHITE, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}




