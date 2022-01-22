/*
  Robotic MainBoard Co-processing Communication

  ESP32-C3 control LED blinking & Basic Stepper motor movement
  RPM (Speed) & Rotate exercise
  
  LEDs
  --  IO2
  --  PA7 (3)(ATTINY1614)

  I2C
  --  IO18 (SCL) <-> PB0 (ATTINY1614)
  --  IO19 (SDA) <-> PB1 (ATTINY1614)


  I2C commands
  -- all i2c write commands have 2 bytes parameter (little endian)
   
   MotorA -> Stepper_1
   MotorB -> Stepper_2
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
   | (version# << 4) or (motorB << 1) or (motorA)    | Retrieve the state of the motors                 |
  
*/


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

byte attinySlaveRcvArray[3];
byte attinySlaveArrayBoard[3];


// Control the LED state of the LED on the
// Robotic MainBoard.
// Input:
//        outputState - HIGH/LOW
// Return:
//        0 - Sucess
//        1 - i2C write failure

int setBoardLED(uint8_t outputState) {
    attinySlaveArrayBoard[0] = 0x02;  // Command 0x02
    attinySlaveArrayBoard[1] = outputState? 0x01:0x00;  // Param1 - LED State
    attinySlaveArrayBoard[2] = 0x00;  // Param2 - Dummy in this case
    delay(10);
    Wire.beginTransmission(I2CADDR_B);
    Wire.write(attinySlaveArrayBoard, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}

// Control the Stepper on the
// Robotic MainBoard.
// Input:
//        motorState - HIGH/LOW
// Return:
//        0 - Sucess
//        1 - i2C write failure

int setMotorRunning(uint8_t motorState) {
    attinySlaveArrayBoard[0] = 0x01;  // Command 0x01
    attinySlaveArrayBoard[1] = motorState? 0x01:0x00;  // Param1 - Stop/Run
    attinySlaveArrayBoard[2] = 0x00;  // Param2 - Dummy in this case
    delay(10);
    Wire.beginTransmission(I2CADDR_B);
    Wire.write(attinySlaveArrayBoard, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}

// Control the Stepper on the
// Robotic MainBoard.
// Input:
//        motor - 0 (Motor A)
//                1 (Motor B)
//        direction - 0 -> Stop 
//                    1 -> Clockwise
//                    2 -> Counter-Clockwise
// Return:
//        0 - Sucess
//        1 - i2C write failure

#define STOP 0
#define CW 1
#define CCW 2

int setDirection(int motor, byte direction) {
    attinySlaveArrayBoard[0] = motor == 0 ? 0x13 : 0x23;  // Command 0x13 or 0x23
    attinySlaveArrayBoard[1] = (direction >= 0) && (direction <= 2) ? direction: 0;  
                                                          // Param1 - Stop/CW/CCW
    attinySlaveArrayBoard[2] = 0x00;  // Param2 - Dummy in this case
    delay(10);
    Wire.beginTransmission(I2CADDR_B);
    Wire.write(attinySlaveArrayBoard, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}



// Control the Stepper on the
// Robotic MainBoard.
// Input:
//        motor - 0 (Motor A)
//                1 (Motor B)
//        revo  - 0 -> Rotate Infinity 
//                Revo -> Revolutions
// Return:
//        0 - Sucess
//        1 - i2C write failure


int rotateMotor(int motor, int revo) {

    attinySlaveArrayBoard[0] = motor == 0 ? 0x11 : 0x21;  // Command 0x11 or 0x21
    attinySlaveArrayBoard[1] = (revo & 0xff);        // Param1 - revolution (L)
    attinySlaveArrayBoard[2] = (revo >> 8) & 0xff;   // Param2 - revolution (H)
    delay(10);
    Wire.beginTransmission(I2CADDR_B);
    Wire.write(attinySlaveArrayBoard, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}



// Control the Stepper on the
// Robotic MainBoard.
// Input:
//        motor - 0 (Motor A)
//                1 (Motor B)
//        rpm(Speed) -  rpm*100 (L)
//        rpm(Speed) -  rpm*100 (H)
//
// Return:
//        0 - Sucess
//        1 - i2C write failure


int setRPM(int motor, float rpm) {

    unsigned int rpm_x_100 = (int) (rpm * 100);
  
    attinySlaveArrayBoard[0] = motor == 0 ? 0x14 : 0x24;  // Command 0x14 or 0x24
    attinySlaveArrayBoard[1] = (rpm_x_100 & 0xff);        // Param1 - rpm*100 (L)
    attinySlaveArrayBoard[2] = (rpm_x_100 >> 8) & 0xff;   // Param2 - rpm*100 (H)
    delay(10);
    Wire.beginTransmission(I2CADDR_B);
    Wire.write(attinySlaveArrayBoard, 3); // Sends 3 bytes i2c to Co-processor.
    if (Wire.endTransmission () == 0) { // Receive 0 = success (ACK response) 
        Serial.println("i2c Write to 0x12 Sucessfull");
        return 0;
    }
    else {
        Serial.println("i2c Write Failed");
        return 1;
    }
}

int checkMotorStatus(int motor) {
    Wire.requestFrom(I2CADDR_B,1);
    attinySlaveRcvArray[0] = Wire.read();
//    Serial.print("i2c Requested Byte: ");
//    Serial.println(attinySlaveRcvArray[0]);
    Wire.endTransmission ();
    return (motor == 1) ? (attinySlaveRcvArray[0] & 0x01) : ((attinySlaveRcvArray[0] >> 1) & 0x01);
}


// the setup function runs once when you press reset or power the board
void setup() {

    Wire.begin(I2C_SDA, I2C_SCL);    
    oled.begin(&Adafruit128x64, OLED_I2CAADR); 
    
    pinMode(LED, OUTPUT);
    delay(2000);
    
    Serial.begin(115200);
    
    attinySlaveArrayBoard[0] = 2;
    attinySlaveArrayBoard[1] = 1;
    attinySlaveArrayBoard[2] = 1;
    
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("MainBoard Co-Processor");
    oled.println("IPC (Interprocess Comm)");

}

// the loop function runs over and over again forever
void loop() {
  
//  Wire.requestFrom(I2CADDR_B,1);
//  attinySlaveRcvArray[0] = Wire.read();
//  Serial.print("i2c Requested Byte: ");
//  Serial.println(attinySlaveRcvArray[0]);
//  Wire.endTransmission ();

    oled.clear();
    oled.println("-LED On (1s)-");
    setBoardLED(HIGH);
    delay(1000);

    oled.clear();
    oled.println("-LED Off (1s)-");
    setBoardLED(LOW);
    delay(1000);

    oled.clear();
    oled.println("-Motor Start FW (5s)-");
    setDirection(0, CCW);
    setDirection(1, CW);
    setMotorRunning(HIGH);
    delay(5000);

    oled.clear();
    oled.println("-Motor Stop (3s)-");
    setMotorRunning(LOW);
    delay(3000);

    oled.clear();
    oled.println("-Motor Start CCW- (5s)");
    setDirection(0, CCW);
    setDirection(1, CCW);
    setMotorRunning(HIGH);
    delay(5000);

    oled.clear();
    oled.println("-Motor Stop (1s)-");
    setMotorRunning(LOW);
    delay(1000);
    oled.clear();

    oled.println("-Motor Start CW- (5s)");
    setDirection(0, CW);
    setDirection(1, CW);
    setMotorRunning(HIGH);
    delay(5000);

    oled.clear();
    oled.println("-Motor Stop (1s)-");
    setMotorRunning(LOW);
    delay(1000);

    oled.println("-Motor Start REW- (5s)");
    setDirection(0, CW);
    setDirection(1, CCW);
    setMotorRunning(HIGH);
    delay(5000);

    oled.clear();
    oled.println("-Motor Stop (1s)-");
    setMotorRunning(LOW);
    delay(1000);


    //------
    oled.clear();
    oled.println("-Motor A 5 steps (1s)-");
    rotateMotor(0, 5);
    delay(1000);

    while(checkMotorStatus(0) == 1) {
        oled.clear();
        oled.println("-Motor still running (1s)-");
        delay (1000);
    }
    oled.println("-Motor DONE running (1s)-");
    delay (1000);


    oled.clear();
    oled.println("-Motor B inf steps (Slow)(1s)-");
    setRPM(1, 5);
    rotateMotor(1, 0);
    setMotorRunning(HIGH);
    delay(1000);

    oled.clear();
    oled.println("-Motor Stop (1s)-");
    setMotorRunning(LOW);
    delay(1000);

}
