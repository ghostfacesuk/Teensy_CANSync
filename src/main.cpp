#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <TimerOne.h>

#define HWSERIAL Serial2
#define GNSSSERIAL Serial8

//LED
const int ledPin = 14; // Dig signal out on screw block terminal
const int ledPin2 = 23; // Buzzer & Red LED
const int ledPin3 = 33; // White LED Only

// Forward Declaration 
void blinkLED(void);
void myInterrupt(void);

// Define CAN bus parameters
const int canSpeed = 500000;  // 500 kbps

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;
CAN_message_t msg;

int LED_Count = 0;
int SerCount = 1;

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
  Timer1.initialize(1000); // MicroSecs = 1ms
  Timer1.attachInterrupt(blinkLED); // blinkLED function 1ms interrupt
  Timer1.stop(); // Stop the timer!
  Serial.begin(115200);
  HWSERIAL.begin(115200);
  Can2.begin();
  Can2.setBaudRate(500000);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(28), myInterrupt, RISING);
}

// The interrupt will happen every 1ms
void blinkLED(void)
{
  LED_Count ++;
  if (LED_Count >= 101) {
    Timer1.stop();
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    LED_Count = 0;
  }
}

void myInterrupt(void) {
 
    //Drive LED High
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    Timer1.start();
    
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
    //Serial.print("PPS Time Stamp "); Serial.println(SerCount);
    HWSERIAL.print("PPS Time Stamp "); HWSERIAL.println(SerCount);

    // Increment Serial message 
    SerCount ++;
}

void loop() {

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