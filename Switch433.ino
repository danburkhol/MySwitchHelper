/*  Arduino Sketch for a MySensors-compatible 433 MHz Transmitter


    Daniel Burkholder 
    Version 1.0
*/

#include <RCSwitch.h>   //433 Transmitter
#include "MySwitch.h"
#include <EEPROM.h>
#include "MySwitchHelpers.h"

#include <MySigningNone.h>
#include <MyTransportNRF24.h>
#include <MyTransportRFM69.h>
#include <MyHwATMega328.h>
#include <MySensor.h>
#include <SPI.h>

#define CHILD_ID 0      //PatioLightFountain
#define TX_PIN 8        //Transmitter PIN#8
#define RX_PIN 1        // Interrupt 0 => Pin #2
                        // Interrupt 1 => Pin #3
int LED_WHITE = 4;
int LED_RED = 5;
int LED_GREEN = 6;

int BITLENGTH = 24;


SWITCH my_switches[NUMBER_OF_SWITCHES] = {
  //{name, on_code, off_code, bitlength}
  {"Front Patio", 5363020, 5363008, 24}, {"Living Room Roto 2", 5592407, 5592404, 24}

};

// RCSwitch rcSwitch = RCSwitch();
MySwitchHelper rcSwitch = MySwitchHelper(TX_PIN, RX_PIN, 10);


// NRFRF24L01 radio driver (set low transmit power by default) 
MyTransportNRF24 radio(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);  
//MyTransportRFM69 radio;
// Message signing driver (none default)
//MySigningNone signer;
// Select AtMega328 hardware profile
MyHwATMega328 hw;
// Construct MySensors library
MySensor gw(radio, hw);


#define SWITCH_1 0            //Child ID of the first switch
#define NUMBER_OF_SWITCHES 2  //Total number of switches

//Declare all your known switches in this array
//Change you number of switches above -> #define NUMBER_OF_SWITCHES
MySwitch mySwitches[NUMBER_OF_SWITCHES] = {
  //MySwitch("Name", On Code , Off Code, bit length (default 24), &rcSwitch)
  //e.g. MySwitch("Front Patio", 5363020, 5363008, 24, &rcSwitch) 
  MySwitch("Front Patio", 5363020, 5363008, 24, rcSwitch), 
  MySwitch("Living Room Roto 2", 5592407, 5592404, 24, rcSwitch)

};




void setup()  {  
  Serial.begin(115200);
  //clearEEPROM();

  //Setup LEDs
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);


  ledWhiteOn();
  //Setup MySensor and assign func for incoming messages from the Controller
  gw.begin(incomingMessage, AUTO, true);
  gw.sendSketchInfo("433Transceiver", "1.0");
  

  ledRedOn();
  // Register all switches with controller (they will be created as child devices)
  presentSwitches();


  ledGreenOn();
  // //Enable Transmission 
  // rcSwitch.enableTransmit(TX_PIN);
  // rcSwitch.setRepeatTransmit(10);
  // //Enable Receiver 
  // rcSwitch.enableReceive(RX_PIN);    // Interrupt 1 => Pin #3


  Serial.println("----- Setup Complete -----");
  ledOff();
}



void loop() {
  //Process any incoming message from Controller
  gw.process();


  //If a 433 MHz transmission is received
  if ( rcSwitch.available() ) {
    //Process this signal
    //processIncomingSignal();
    
    //Reset switch and prepare for the next signal
    //rcSwitch.resetAvailable();


    matchSwitchTest();

  }
  
  ledOff();
} 



void presentSwitches() {
  //Present the array of switches to the Controller
  Serial.println("----- Begin Presentation -----");
  
  //Iterate through each known SWITCH, Present to Controller
  // for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
  //   Serial.println("Presenting: ");
  //   Serial.println(mySwitches[i].getName());
    
  //   gw.present(i, S_LIGHT);
  // }

  SWITCH *a = rcSwitch.getSwitches();

  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    Serial.println("Presenting: ");
    Serial.print( a[i].name );
    
    gw.present(i, S_LIGHT);
  }

  Serial.println("----- End Presentation -----");
}



//Process incoming 433 MHz signals
//DEPRECATED ********
void processIncomingSignal() {
  Serial.println("----- 433 Signal Received -----");

  ledWhiteOn();

  unsigned long value = rcSwitch.getReceivedValue();

  //For Debugging or Sniffing
  printSignal();

  //Match signal to known codes and update Controller if necessary
  matchSignalUpdateController(value);

}

//Compare the incoming value to known Switch Values
//If there is a match, update the Controller with new state

//DEPRECATED ********

void matchSignalUpdateController(unsigned long value) {
  
  Serial.println("----- MySwitch Matching -----");
  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    //If the value matches either on code or off code
    //Update the controller with the new state

    if (mySwitches[i].match(value)) {
      Serial.println("Match! Updating controller with new state");
      updateController(i, mySwitches[i].matchSignal(value));

      break;
    
    }

    Serial.println("NO MATCH");
  }

}

//DEPRECATED ********
void printSignal() {
  unsigned long value = rcSwitch.getReceivedValue();

  if (value == 0) {
    Serial.print("Unknown encoding");
  } else {
    Serial.print("Received ");
    Serial.print( rcSwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( rcSwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print(" Recev Delay: ");
    Serial.print(rcSwitch.getReceivedDelay());
    Serial.print("  Protocol: ");
    Serial.println( rcSwitch.getReceivedProtocol() );
  }

}


//
void matchSwitchTest() {
  matched_switch m = rcSwitch.matchSwitch();
  if (m.childId != 255) {

    updateController(m.childId, m.newState);

  }
}


//
void updateController(int childSensorId, int newState) {
  digitalWrite(LED_GREEN, HIGH);
  MyMessage updateMsg(childSensorId, V_LIGHT);
  updateMsg.set(newState);

  gw.send(updateMsg);

}

//====================


//Incoming Message from the Controller
void incomingMessage(const MyMessage &message) {
  Serial.println("----- MSG FROM CONTROLLER -----");

  if (message.type==V_LIGHT) {
    ledGreenOn();

    // Write some debug info

    //SWITCH *a = rcSwitch.getSwitches();

    // Serial.print( mySwitches[message.sensor].getName() );
    // Serial.print( rcSwitch.getSwitches()[message.sensor].name );
    // Serial.print(", New status: ");
    // Serial.println(message.getBool());


    // Transmit 433 Switch command to specified switch
    // transmitSwitchCommand(message.sensor, message.getBool());
    
    rcSwitch.transmitSwitchCmd(message.sensor, message.getBool());
   } 

   ledGreenOff();
}


//**** DEPREC *****
void transmitSwitchCmd(int childId, int state) {
  Serial.println("TransmistSwitchCmd");
  digitalWrite(LED_RED, HIGH);


  if (state == 1) {
    //mySwitches[childId].on();
    rcSwitch.send(mySwitches[childId].getOnCode(), 24);


  }else {
    //mySwitches[childId].off();

    rcSwitch.send(mySwitches[childId].getOffCode(), 24);
  }


  digitalWrite(LED_RED, LOW);

}

// Transmits a command (ON / OFF) to a predefined 433MHz Switch
// Comand is recv from the Controller

//****** DEPREC *****
void transmitSwitchCommand(int childId, bool state) {
  digitalWrite(LED_RED, HIGH);

  mySwitches[childId].send(state);

  digitalWrite(LED_RED, LOW);
}

void transmitSwitchCommand(int childId, int state) {
  digitalWrite(LED_RED, HIGH);

  mySwitches[childId].send(state);

  digitalWrite(LED_RED, LOW);
}


void clearEEPROM() {
  Serial.println("Clear EEPROM");
    // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0x0);
    
  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}

//LED funcs

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




