                    
/*
  Robotic Core Board Co-processing Communication

  ESP32-C3 control LED blinking & Audio
  Use the i2C master to command our Slave device to generate Old McDonald tune.
  
  
  LEDs
  --  IO2
  --  PA7 (3)(ATTINY1614)

  I2C
  --  IO18 (SCL) <-> PB0 (ATTINY1614)
  --  IO19 (SDA) <-> PB1 (ATTINY1614)


  I2C commands
  -- all i2c write commands have 2 bytes parameter (little endian)
   
   MotorA -> Left Motor
   MotorB -> Right Motor
   Write:
   |   commands         | param1   | param2   |  description                                            |
   |--------------------|----------|----------|---------------------------------------------------------|
   | 0x00 | ParamInit   | dummy    | dummy    | Set motor profile to default value                      |
   | 0x01 | Stop/Run    | Stop/Run | dummy    | Stop/Run both stepper motors.                           |
   |      | Both Motors |          |          |                                                         |
   | 0x02 | Enable      | 0/1      | dummy    | PA7 (3) LED.                                            |
   |      |             |          |          |                                                         |   
   |      |             |          |          |     Motor A                                             |   
   | 0x11 | Rotate(Rev) | rev(L)   | rev(H)   | Move number of revolution                               |
   |      |             |          |          |  if number is 0x0000 move infinity steps (continuous)   |
   | 0x12 | Stop        | dummy    | dummy    | Stop Motor (Same as sending Stop(0) to Direction        |
   | 0x13 | Direction   | Stop/Dir | dummy    | Accept the following: STOP->0 CW->1 CCW->2              |
   | 0x14 | RPM (x100)  | rpm(L)   | rpm(H)   | Set the Round per minutes x 100 of the Motor            |
   | 0x15 | SPR         | spr(L)   | spr(H)   | Set Steps Per Rotation of the motor of the motor        |
   |      |             |          |          |     default is 0 -> 4075.7728395                        | 
   | 0x16 | Rotate(Deg) | deg(L)   | deg(H)   | Rotate the motor a given number of degrees (x10)        |
   |      | (x10)       |          |          |                                                         |   
   |      |             |          |          |                                                         |   
   |      |             |          |          |     Motor - B                                           |
   | 0x21 | Rotate(Rev) | rev(L)   | rev(H)   | Move number of revolution                               |
   |      |             |          |          |  if number is 0x0000 move infinity steps (continuous)   |
   | 0x22 | Stop        | dummy    | dummy    | Stop Motor (Same as sending Stop(0) to Direction        |
   | 0x23 | Direction   | Stop/Dir | dummy    | Accept the following: STOP->0 CW->1 CCW->2              |
   | 0x24 | RPM (x100)  | rpm(L)   | rpm(H)   | Set the Round per minutes x 100 of the Motor            |
   | 0x25 | SPR         | spr(L)   | spr(H)   | Set Steps Per Rotation of the motor of the motor        |
   |      |             |          |          |     default is 0 -> 4075.7728395                        | 
   | 0x26 | Rotate(Deg) | deg(L)   | deg(H)   | Rotate the motor a given number of degrees (x10)        |
   |      | (x10)       |          |          |                                                         |   
    
   Read:
   |                       param1                    |  description                                     |
   |-------------------------------------------------|--------------------------------------------------|
   | (version# << 4) or (motorA << 1) or (motorB)    | Retrieve the state of the motors                 |
  
*/

#include "pitches.h"

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define SR04_I2CADDR 0x57
#define OLED_I2CAADR 0x3C

// i2c PCF8574(A) Port expander
// PCF8574  -> 0 0 1 0  0 A2 A1 A0    0 0 1 1
// PCF8574A -> 0 0 1 1  1 A2 A1 A0    1 0 1 1
#define PEXP_I2CADDR 0x23

// i2c Slave Co-processor - On the Core-Module
#define I2CADDR 0x13
// i2c Slave Co-processor - On the Robot MainBoard
#define I2CADDR_B 0x12

#define I2C_SDA 19
#define I2C_SCL 18
#define LED 2
#define IR_RECV 4
#define NEO_PIXEL 5
#define LED_COUNT 3

SSD1306AsciiWire oled;

byte attinySlaveArray[3];


// Control the LED state of the LED on the
// Robotic MainBoard.
// Input:
//        outputState - HIGH/LOW
// Return:
//        0 - Sucess
//        1 - i2C write failure

int setBoardLED(uint8_t outputState) {
    attinySlaveArray[0] = 0x02;  // Command 0x02 Turn on/off the LED PA1
    attinySlaveArray[1] = outputState? 0x01:0x00;  // Param1 - LED State
    attinySlaveArray[2] = 0x00;  // Param2 - Dummy in this case
    delay(10);
    Wire.beginTransmission(I2CADDR);
    Wire.write(attinySlaveArray, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}


int setFreq(unsigned int frequency) {
    attinySlaveArray[0] = 0x10;  // Command 0x10
    attinySlaveArray[1] = (frequency & 0xff);        // Param1 - freq (L)
    attinySlaveArray[2] = (frequency >> 8) & 0xff;   // Param2 - freq (H)
    delay(10);
    Wire.beginTransmission(I2CADDR);
    Wire.write(attinySlaveArray, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}


int setToneDuration(unsigned int duration) {
    attinySlaveArray[0] = 0x11;  // Command 0x11
    attinySlaveArray[1] = (duration & 0xff);        // Param1 - duration (L)
    attinySlaveArray[2] = (duration >> 8) & 0xff;   // Param2 - duration (H)
    delay(10);
    Wire.beginTransmission(I2CADDR);
    Wire.write(attinySlaveArray, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}


void playNote(unsigned int frequency, unsigned int duration) {
  setFreq(frequency);
  setToneDuration(duration * 0.9);
  delay(duration); 
}


// the setup function runs once when you press reset or power the board
void setup() {

    Wire.begin(I2C_SDA, I2C_SCL);    
    oled.begin(&Adafruit128x64, OLED_I2CAADR); 
    
    pinMode(LED, OUTPUT);
    delay(2000);
    
    Serial.begin(115200);
    
    attinySlaveArray[0] = 2;
    attinySlaveArray[1] = 1;
    attinySlaveArray[2] = 1;
    
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("CoreBoard Co-Processor");


    playNote(NOTE_F4, 500);
    playNote(NOTE_F4, 500);
    playNote(NOTE_F4, 500);
    playNote(NOTE_C4, 500);
    playNote(NOTE_D4, 500);
    playNote(NOTE_D4, 500);
    playNote(NOTE_C4, 1000);
    playNote(NOTE_A4, 500);
    playNote(NOTE_A4, 500);
    playNote(NOTE_G4, 500);
    playNote(NOTE_G4, 500);
    playNote(NOTE_F4, 1500);
    
}

// the loop function runs over and over again forever
void loop() {
  
    oled.clear();
    oled.println("-LED On (1s)-");
    setBoardLED(HIGH);
    delay(1000);

    oled.clear();
    oled.println("-LED Off (1s)-");
    setBoardLED(LOW);
    delay(1000);

}
                    
                
