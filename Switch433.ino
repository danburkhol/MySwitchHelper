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

#define SWITCH_ON 1            //V_LIGHT values 1 / 0 for ON / OFF
#define SWITCH_OFF 0

#define SWITCH_1 0            //Child ID of the first switch
#define NUMBER_OF_SWITCHES 2  //Total number of switches


#define ON 1
#define OFF 0

int BITLENGTH = 24;




RCSwitch rcSwitch = RCSwitch();

//Declare all your known switches in this array
//Change you number of switches above -> #define NUMBER_OF_SWITCHES
static SWITCH mySwitches[NUMBER_OF_SWITCHES] = {
  //{name, on_code, off_code, bitlength}
  {"Front Patio", 5363020, 5363008, 24}, {"Living Room Roto 2", 5592407, 5592404, 24}

};

MySwitch switches [NUMBER_OF_SWITCHES];

MySensor gw; 
MyMessage msg(CHILD_ID, V_LIGHT);


void setup()  {  
  Serial.begin(115200);
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
    // MySwitch t("Front Patio", 5363020, 5363008, 24, &rcSwitch);
    switches[i] = MySwitch(&mySwitches[i].name, &mySwitches[i].on_code, &mySwitches[i].off_code, &mySwitches[i].bitlength, &rcSwitch);
   
    MySwitch t(&mySwitches[i].name, &mySwitches[i].on_code, &mySwitches[i].off_code, &mySwitches[i].bitlength, &rcSwitch);
    t.printSwitch();
    t.transmit24bit();
    //t.transmit24bit(&rcSwitch);
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
  
} 




void presentSwitches() {
  //Present the potential array of switches to the Controller
  Serial.println("Begin Presentation");
  
  
  //Iterate through each known SWITCH, Present to Controller
  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    Serial.println("Presenting: ");
    Serial.println(mySwitches[i].name);
    
    gw.present(i, S_LIGHT);
  }

}



void procIncSignalTest() {
  //Time since the first Signal was rcv;
  unsigned long timeSignalRecv = millis();
  unsigned long delayAdd = 2000;   //2000ms = 2s delay
  unsigned long delayedTime = timeSignalRecv + delayAdd;

  unsigned long valueArray[25];
  int i = 0;
  
  Serial.println("Inital Time Recv / Delayed Time");
  Serial.println(timeSignalRecv);
  Serial.println(delayedTime);

  digitalWrite(LED_RED, HIGH);
  //Gather signals and keep monitoring for new 
  //signals for up to two seconds after the last signal was recv
  while ( delayedTime > millis() ) {
    
    

    if ( rcSwitch.available() ) {
      digitalWrite(LED_GREEN, HIGH);
      //Save the value to array and reset immediately
      valueArray[i] = rcSwitch.getReceivedValue();  
      Serial.println("Recv Signal");
      Serial.println(valueArray[i]);
      i++;

      rcSwitch.resetAvailable();

      //Reset the times so it will continue to loop for two 
      //more seconds since the last signal was recv
      timeSignalRecv = millis();
      delayedTime = timeSignalRecv + delayAdd;

      
    }

    digitalWrite(LED_GREEN, LOW);
  }


  printSignal2( valueArray );


}


void printSignal2(unsigned long valueArray[25]) {

  for (int i = 0; i < 25; i++ ){
    Serial.println(valueArray[i]);

  }

}



//Process incoming 433 MHz signals
void processIncomingSignal() {
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

void matchSignalUpdateController(unsigned long value) {
  //Compare the incoming value to known Switch Values

  for (int i = 0; i < NUMBER_OF_SWITCHES; i++) {
    Serial.println("----- MySwitch Matching ------");

    //If the value matches either on code or off code
    //Update the controller with the new state
    if (switches[i].match(value)) {
      Serial.println("Match! Updating controller with new state");
      //digitalWrite(LED_GREEN, HIGH);
      updateController(i, switches[i].matchSignal(value));

      break;
    }


  }

  Serial.println("No Match");
  digitalWrite(LED_RED, HIGH);

}

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


void updateController(int childSensorId, int newState) {
  digitalWrite(LED_GREEN, HIGH);
  MyMessage updateMsg(childSensorId, V_LIGHT);
  updateMsg.set(newState);

  gw.send(updateMsg);

}



//Incoming Message from the Controller
void incomingMessage(const MyMessage &message) {

  //For Debugging messages from Controller
  //printMessage(message);

  transmitToChildNode(message.sensor, message.getInt());
  

}

void printMessage(MyMessage message) {
  Serial.print("Message from controller recieved");
  Serial.print("Sensor number: ");
  Serial.print(message.sensor);
  Serial.print(" // Receieve Value: ");
  Serial.print(message.getInt());

}

/*Transmits a command (ON / OFF) to a predefined 433MHz Switch
  Comand is recv from the Controller

*/
void transmitToChildNode(int childId, int state) {

  digitalWrite(LED_RED, HIGH);
  switch (state) {
    case 0:
      transmit(mySwitches[childId].off_code, 24);
      break;

    case 1:
      transmit(mySwitches[childId].on_code, 24);
      break;

  }
  digitalWrite(LED_RED, LOW);

}

void transmit(unsigned long code, int bitlength) {
  Serial.println("Beginning Transmission");
  Serial.println(code);
  Serial.println(" // Bit Length  ");
  Serial.println(bitlength);

  rcSwitch.send(code, bitlength);
  
  Serial.println("Transmission Complete");
}


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




void transmit(unsigned long code) {
  rcSwitch.send(code, BITLENGTH);
  Serial.println("Tx Complete");
}




