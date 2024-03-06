#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <TimerOne.h>

#define HWSERIAL Serial2
#define GNSSSERIAL Serial8

// LED, Buzzer, Digout
const int ledPin = 14; // Dig signal out on screw block terminal
const int ledPin2 = 23; // Buzzer & Red LED
const int ledPin3 = 33; // White LED Only

// Timer tracker
int TimerStatus = 1;

volatile bool sendCanMsg = false;
volatile uint32_t canCounter = 0;

// Define CAN bus parameters
const int canSpeed = 500000;  // 500 kbps

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;
CAN_message_t msg;

void blinkLED() {
  static int LED_Count = 0;
  LED_Count++;

  if (LED_Count >= 30) {
    TimerStatus = 0;
  }

  if (LED_Count >= 101) {
    Timer1.stop();
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    LED_Count = 0;
  }
}

void myInterrupt() {
  if (!sendCanMsg && TimerStatus == 0 ) {
    sendCanMsg = true;
    Timer1.start();
    TimerStatus = 1;
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  HWSERIAL.begin(115200);
  Can2.begin();
  Can2.setBaudRate(500000);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(28), myInterrupt, RISING);
  Timer1.initialize(1000); // MicroSecs = 1ms
  Timer1.attachInterrupt(blinkLED); // blinkLED function 1ms interrupt
  Timer1.start(); // Start the timer!
}

void loop() {
  if (sendCanMsg) {
    CAN_message_t canMsg;
    canMsg.id = 0x123;  // CAN ID
    canMsg.len = 8;     // Message length (8 bytes)
    canCounter++;
    canMsg.buf[0] = (canCounter >> 24) & 0xFF;
    canMsg.buf[1] = (canCounter >> 16) & 0xFF;
    canMsg.buf[2] = (canCounter >> 8) & 0xFF;
    canMsg.buf[3] = canCounter & 0xFF;
    canMsg.buf[4] = canMsg.buf[5] = canMsg.buf[6] = canMsg.buf[7] = 0x00;  
    Can2.write(canMsg);

    // Serial message 
    HWSERIAL.print("PPS Time Stamp ");
    HWSERIAL.println(canCounter);

    sendCanMsg = false;
  }

  while (Can2.read(msg)) {
    Serial.print("CAN1 "); 
    Serial.print("MB: "); Serial.print(msg.mb);
    Serial.print("  ID: 0x"); Serial.print(msg.id, HEX );
    Serial.print("  EXT: "); Serial.print(msg.flags.extended );
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" DATA: ");
    for (uint8_t i = 0; i < 8; i++) {
      Serial.print(msg.buf[i]); Serial.print(" ");
    }
    Serial.print("  TS: "); Serial.println(msg.timestamp);
  }
}