#include <Arduino.h>
#include <FlexCAN_T4.h>

//LED
const int ledPin = 33;

// Define CAN bus parameters
const int canSpeed = 500000;  // 500 kbps

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;
CAN_message_t msg;

int LED_State = 0;
int counter_ms = 0;

//Used for CAN counter increments
union _myunion
{
  /* data */
  uint32_t u32data;
  uint8_t u8data[4];
  float f32;
}
myunion;

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

  if (++counter_ms >= 1000) {
    counter_ms = 0;
  }

  // Create a CAN message
  CAN_message_t canMsg;
  canMsg.id = 0x123;  // CAN ID
  canMsg.len = 8;     // Message length (8 bytes)
  
  // Fill data bytes with some values
//  for (int i = 0; i < 8; i++) {
//    canMsg.buf[i] = i;
//  }

  //Serial.println("SG Test");

  if (counter_ms == 0) {
  // set the LED on
  digitalWrite(ledPin, HIGH);
    // Send the CAN message

  myunion.u32data++;
  canMsg.buf[0] = myunion.u8data[0];
  canMsg.buf[1] = myunion.u8data[1];
  canMsg.buf[2] = myunion.u8data[2];
  canMsg.buf[3] = myunion.u8data[3];
  canMsg.buf[4] = canMsg.buf[5] = canMsg.buf[6] = canMsg.buf[7] = 0x00;  
  Can2.write(canMsg);
  } 
  if (counter_ms == 100) {
  digitalWrite(ledPin, LOW);
  }
  /*
    while ( Can2.read(msg) ) {
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
*/
  // Wait for one second before sending the next frame
  delay(1);
}