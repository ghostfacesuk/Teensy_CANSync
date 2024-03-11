#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <TimerOne.h>

#define HWSERIAL Serial2
#define GNSSSERIAL Serial8

// LED, Buzzer, Digout
const u_int8_t ledPinA = 14; // Dig signal out on screw block terminal
const u_int8_t ledPinB = 23; // Buzzer & Red LED
const u_int8_t ledPinC = 33; // White LED Only

// Timer & CAN msg flags
volatile bool TimerStatus = false;
volatile bool sendCanMsg = false;
volatile uint32_t canCounter = 0;

// Define CAN bus parameters
const uint32_t canSpeed = 500000;  // 500 kbps

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;
CAN_message_t msg;

void blinkLED() {
    static uint_fast16_t LED_Count = 0;
    LED_Count++;

    if (LED_Count >= 101) {
        digitalWrite(ledPinA, LOW);
        digitalWrite(ledPinB, LOW);
        digitalWrite(ledPinC, LOW);
        if (LED_Count >= 800) {
            Timer1.stop();
            TimerStatus = false;
            LED_Count = 0;
        }
    }
}

void myInterrupt() {
    if (!sendCanMsg && !TimerStatus) {
        sendCanMsg = true;
        TimerStatus = true;
        digitalWrite(ledPinA, HIGH);
        digitalWrite(ledPinB, HIGH);
        digitalWrite(ledPinC, HIGH);
        Timer1.start();
    }
}

void setup() {
    Serial.begin(115200);
    HWSERIAL.begin(115200);
    Can2.begin();
    Can2.setBaudRate(500000);
    pinMode(ledPinA, OUTPUT);
    pinMode(ledPinB, OUTPUT);
    pinMode(ledPinC, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(28), myInterrupt, RISING);
    Timer1.initialize(1000);           // MicroSecs = 1ms
    Timer1.attachInterrupt(blinkLED);  // blinkLED function 1ms interrupt
    Timer1.start();                    // Start the timer!
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