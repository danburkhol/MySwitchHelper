/*  Arduino Sketch for a MySensors-compatible 433 MHz Transmitter


    Daniel Burkholder 
    Version 1.0
*/

#include <MySensor.h>   //MySensors 
#include <SPI.h>
#include <RCSwitch.h>   //433 Transmitter

#define CHILD_ID 1      //PatioLightFountain
#define TX_PIN 8        //Transmitter PIN#8
#define RX_PIN 1        // Interrupt 0 => Pin #2
                        // Interrupt 1 => Pin #3

#define ON 1
#define OFF 0


//Switch Codes - Cannot use #define values in the RCSwitch.send(int,int) func for some reason. Declare them as Ints
//Patio / Fountain Switch Codes
int PATIO_ON = 5363020;
int PATIO_OFF = 5363008;
int BITLENGTH = 24;

int oldState;
int currentState;


RCSwitch mySwitch = RCSwitch();

MySensor gw; 
MyMessage msg(CHILD_ID, V_LIGHT);


void setup()  {  
  //Enable Transmission 
  mySwitch.enableTransmit(TX_PIN);
  //Enable Receiver 
  mySwitch.enableReceive(RX_PIN);    // Interrupt 1 => Pin #3
  
  //Setup MySensor and assign func for incoming messages from the Controller
  gw.begin(incomingMessage, AUTO, true);
  gw.sendSketchInfo("Switch433Custom", "1.0");
  
  // Register binary input sensor to gw (they will be created as child devices)
  gw.present(CHILD_ID, S_LIGHT);   // Present each Child Sensor to the Controller


}


//  Check if digital input has changed and send in new value
void loop() {
  
  process433();   //Process any incoming messages from Controller

  gw.process();   //Process any incoming messages from Controller

} 


//Incoming 433 MHz Signal
void process433() {
  if (mySwitch.available()) {
    
    int value = mySwitch.getReceivedValue();
    
    if (value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print( mySwitch.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( mySwitch.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( mySwitch.getReceivedProtocol() );
    }

    mySwitch.resetAvailable();
  }
  
}


//Incoming Message from the Controller
void incomingMessage(const MyMessage &message) {

  //TODO IF V_LIGHT

  Serial.println("V_LIGHT command recv: ");
  Serial.print(message.getBool());
  
  
  switch (message.getBool()) {
    case 0:
      patioOff();
      break;

    case 1:
      patioOn();
      break;
    
  }
  
}




void updateState(int NEW_STATE) {
  //TODO REQUIRE CHILD_ID BEING UPDATED

  Serial.println("Updating State to Controller");
  gw.send( msg.set(NEW_STATE) );

}

void patioOn() {
  Serial.println("Switching on!");
  mySwitch.send(5363020, 24);
  currentState = ON;
}

void patioOff() {
  Serial.println("Switcing off");
  mySwitch.send(5363008, 24);
  currentState = OFF;
}

void transmit(int code) {
  mySwitch.send(code, BITLENGTH);
  Serial.println("Tx Complete");
}




