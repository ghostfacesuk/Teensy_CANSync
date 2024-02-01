#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <TimerOne.h>

//LED
const int ledPin = 33;

// Forward Declaration 
void blinkLED(void);
void myInterrupt(void);

// Define CAN bus parameters
const int canSpeed = 500000;  // 500 kbps

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;
CAN_message_t msg;

int LED_State = 0;
int counter_ms = 0;
int bob = 0;

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
  Timer1.initialize(1000);
  Serial.begin(115200);
  Can2.begin();
  Can2.setBaudRate(500000);
  Timer1.attachInterrupt(blinkLED); // blinkLED to run every 0.15 seconds
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(32), myInterrupt, RISING);
}

// The interrupt will blink the LED
void blinkLED(void)
{
  if (++counter_ms >= 1000) {
    counter_ms = 0;
    bob=1;
  }
  if (counter_ms <= 100) {
    digitalWrite(ledPin, LOW);
  }
}

void myInterrupt(void) {
 
    //Drive LED High
    digitalWrite(ledPin, HIGH);
    
    // Create a CAN message
    CAN_message_t canMsg;
    canMsg.id = 0x123;  // CAN ID
    canMsg.len = 8;     // Message length (8 bytes)
    myunion.u32data++;
    canMsg.buf[0] = myunion.u8data[0];
    canMsg.buf[1] = myunion.u8data[1];
    canMsg.buf[2] = myunion.u8data[2];
    canMsg.buf[3] = myunion.u8data[3];
    canMsg.buf[4] = canMsg.buf[5] = canMsg.buf[6] = canMsg.buf[7] = 0x00;  
    // Send the CAN message
    Can2.write(canMsg);

    // Send a serial message
    Serial.print("PPS Time Stamp\r\n");
}

void loop() {

  // LED toggle
  if (bob == 1) {
    bob = 0;
  } 

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
}