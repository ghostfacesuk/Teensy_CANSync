#include <Arduino.h>
#include <FlexCAN_T4.h>

//LED
const int ledPin = 33;

// Define CAN bus parameters
const int canSpeed = 500000;  // 500 kbps

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;
CAN_message_t msg;

void setup() {
  Serial.begin(115200);
  //Serial.println("Hello world");
  // Initialize CAN bus
  Can2.begin();
  Can2.setBaudRate(500000);
  // Initialize the digital pin as an output. 
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Create a CAN message
  CAN_message_t canMsg;
  canMsg.id = 0x123;  // CAN ID
  canMsg.len = 8;     // Message length (8 bytes)
  
  // Fill data bytes with some values
  for (int i = 0; i < 8; i++) {
    canMsg.buf[i] = i;
  }
  
  // Send the CAN message
  Can2.write(canMsg);

  //Serial.println("SG Test");

  // set the LED on
  digitalWrite(ledPin, HIGH);

    if ( Can2.read(msg) ) {
    Serial.print("CAN1 "); 
    Serial.print("MB: "); Serial.print(msg.mb);
    Serial.print("  ID: 0x"); Serial.print(msg.id, HEX );
    Serial.print("  EXT: "); Serial.print(msg.flags.extended );
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" DATA: ");
    for ( uint8_t i = 0; i < 8; i++ ) {
      Serial.print(msg.buf[i]); Serial.print(" ");
    }
    Serial.print("  TS: "); Serial.println(msg.timestamp);
  }

  // Wait for one second before sending the next frame
  delay(500);

  // set the LED off
  digitalWrite(ledPin, LOW);

  // Wait for one second before sending the next frame
  delay(500);
}