/*  Arduino Sketch for a MySensors-compatible 433 MHz Transmitter


    Daniel Burkholder 
    Version 1.0
*/

#include <MySensor.h>   //MySensors 
#include <SPI.h>
#include <RCSwitch.h>   //433 Transmitter
#include "Switch433Types.h"
#include "MySwitch.h"

#define CHILD_ID 0      //PatioLightFountain
#define TX_PIN 8        //Transmitter PIN#8
#define RX_PIN 1        // Interrupt 0 => Pin #2
                        // Interrupt 1 => Pin #3
int LED_WHITE = 4;
int LED_RED = 5;
int LED_GREEN = 6;

int BITLENGTH = 24;


RCSwitch rcSwitch = RCSwitch();
MySensor gw; 

#define SWITCH_1 0            //Child ID of the first switch
#define NUMBER_OF_SWITCHES 2  //Total number of switches

//Declare all your known switches in this array
//Change you number of switches above -> #define NUMBER_OF_SWITCHES
MySwitch mySwitches[NUMBER_OF_SWITCHES] = {
  //MySwitch("Name", On Code , Off Code, bit length (default 24), &rcSwitch)
  //e.g. MySwitch("Front Patio", 5363020, 5363008, 24, &rcSwitch) 
  MySwitch("Front Patio", 5363020, 5363008, 24, &rcSwitch), 
  MySwitch("Living Room Roto 2", 5592407, 5592404, 24, &rcSwitch)

};



void setup()  {  
  Serial.begin(115200);
  //Setup LEDs
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  digitalWrite(LED_WHITE, HIGH);


  //Setup MySensor and assign func for incoming messages from the Controller
  gw.begin(incomingMessage, AUTO, true);
  gw.sendSketchInfo("Switch433Custom", "1.0");
  
  digitalWrite(LED_RED, HIGH);
  // Register binary input sensor to gw (they will be created as child devices)
  //gw.present(CHILD_ID, S_LIGHT);   // Present each Child Sensor to the Controller
  presentSwitches();


  digitalWrite(LED_GREEN, HIGH);
  //Enable Transmission 
  rcSwitch.enableTransmit(TX_PIN);
  rcSwitch.setRepeatTransmit(10);
  //Enable Receiver 
  rcSwitch.enableReceive(RX_PIN);    // Interrupt 1 => Pin #3
  


  Serial.println("----TESTING MySwitch class ------ ");
  //Testing MySwitch.h and .cpp
  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    mySwitches[i].printSwitch();

  }


  Serial.println("----TESTING MySwitch class ------ ");

  digitalWrite(LED_WHITE, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}


//  Check if digital input has changed and send in new value
void loop() {

  //If a 433 MHz transmission is received
  if ( rcSwitch.available() ) {
    //Process this signal
    processIncomingSignal();
    
    //Reset switch and prepare for the next signal
    rcSwitch.resetAvailable();

  }else {
    //Serial.println("Else GW Process");
    //Process any incoming messages from Controller. 
    //Runs the incomingMessage(MyMessage) function
    //gw.process();  

    //Serial.println("ELSE Count");
    //Serial.println(count);



  }
  
  gw.process();

  //sleep(uint8_t interrupt1, uint8_t mode1, uint8_t interrupt2, uint8_t mode2, unsigned long ms=0);
  
} 




void presentSwitches() {
  //Present the array of switches to the Controller
  Serial.println("----- Begin Presentation -----");
  
  //Iterate through each known SWITCH, Present to Controller
  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    Serial.println("Presenting: ");
    Serial.println(mySwitches[i].getName());
    
    gw.present(i, S_LIGHT);
  }

  Serial.println("----- End Presentation -----");
}



//Process incoming 433 MHz signals
void processIncomingSignal() {
  Serial.println("----- Signal Received -----");

  digitalWrite(LED_WHITE, HIGH);
  unsigned long value = rcSwitch.getReceivedValue();

  //For Debugging or Sniffing
  printSignal();


  //Match signal to known codes and update Controller if necessary
  matchSignalUpdateController(value);


  digitalWrite(LED_WHITE, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

//
void matchSignalUpdateController(unsigned long value) {
  //Compare the incoming value to known Switch Values

  Serial.println("-- MySwitch Matching --");
  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    //If the value matches either on code or off code
    //Update the controller with the new state
    if (mySwitches[i].match(value)) {
      Serial.println("Match! Updating controller with new state");
      //digitalWrite(LED_GREEN, HIGH);
      updateController(i, mySwitches[i].matchSignal(value));

      break;
    }

    Serial.println("NO MATCH");
  }


}

//
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
void updateController(int childSensorId, int newState) {
  digitalWrite(LED_GREEN, HIGH);
  MyMessage updateMsg(childSensorId, V_LIGHT);
  updateMsg.set(newState);

  gw.send(updateMsg);

}

//====================


//Incoming Message from the Controller
void incomingMessage(const MyMessage &message) {

  //printMessage(message);
  if (message.type==V_LIGHT) {
    // Write some debug info
    Serial.print("Incoming change for sensor:");
    Serial.print(mySwitches[message.sensor].getName());
    Serial.print(", New status: ");
    Serial.println(message.getBool());


    // Transmit 433 Switch command to specified switch
    transmitSwitchCommand(message.sensor, message.getBool());
    
   } 

}



// Transmits a command (ON / OFF) to a predefined 433MHz Switch
// Comand is recv from the Controller
void transmitSwitchCommand(int childId, bool state) {
  digitalWrite(LED_RED, HIGH);

  mySwitches[childId].send(state);

  digitalWrite(LED_RED, LOW);
}

// void transmit(unsigned long code, int bitlength) {
//   Serial.println("Beginning Transmission");
//   Serial.println(code);
//   Serial.println(" // Bit Length  ");
//   Serial.println(bitlength);

//   rcSwitch.send(code, bitlength);
  
//   Serial.println("Transmission Complete");
// }


//OLD, superseceded by updateController
//
//Used to update the controller if there is an external state change of a 433 MHz switch
// e.g. Someone uses the original remote to turn it off and not the Controller
//getbool() runs getint() -- maybe switch to getInt() when procsseing to simplify
//newState is boolean because received messages report the new state of V_LIGHT as bool
// void updateStateToController(int childSensorId, int variableType, bool newState) {
//   //Create a message for the child sensor
//   MyMessage updateMsg(childSensorId, variableType);

//   //Set the new state of this child
//   if (newState) {
//     updateMsg.set(ON);

//   }else {
//     updateMsg.set(OFF);

//   }
  
//   //Send update to Controller
//   gw.send(updateMsg);
// }






