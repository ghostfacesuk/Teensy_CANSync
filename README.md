# Teensy_CANSync

## Teensy 4.1
* Pin 14 is DIG OUT to Screw terminal block
* Pin 23 is Red LED + Buzzer
* Pin 28 is PPS REF from uBlox
* Pin 33 is White LED
* Pins 7 & 8 UART (RX2 to RX, TX2 to TX on TTL RS232 Converter)
* Pins 35 & 35 UART (TX to RX, Rx to TX on uBlox F9 board)
* Pins 0 & 1 CAN2 (CRX2 to RX, CTX2 to TX on CAN Transceiver)

## How to use
* Connect GPS antenna to uBlox SMA connector
* Connect USB Micro cable to PC (used for powering the Teensy)
* connect Digital wire to screw terminal block (optional)
* Connect CAN and Serial cables to 9-way D-type connectors (optional)
* Swap jumper position from perpendicular position to parallel position to enable Red LED and Buzzer (optional) 

## Config & Behaviour
* CAN bus will be fixed to 500kbps - 0x123 will be transmitted when PPS input is detected
* RS232 will be fixed to 115200 - PPS Time Stamp XX will be transmitted when PPS input is detected
* Pins 14, 23, and 33 will be driven HIGH when PPS input is detected (attached to LEDs, block, and Buzzer)

## Latency Info
* CAN - PPS to start of CAN message transmit = 3.2 Microseconds
* RS232 - PPS to start of serial message transmit = 2.4 Microseconds 
* LED/DIG - PPS to DIG out (LEDs) = 200 Nanoseconds 