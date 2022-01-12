/* 
 *  Ex_11 - Misc i2C Range Sensor
 *  Retrieve Range information from the i2C Ultrasonic Sensor
 *  Todo: Add in the VL6180X library & code
 *  
 *    LEDs 
 *    --  IO2
 *    
 *    
 *    I2C 
 *    --  IO18 (SCL) <-> PB0 (ATTINY1614  
 *    --  IO19 (SDA) <-> PB1 (ATTINY1614)
 *         
 *    
 */
 
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// I2C address
#define SR04_I2CADDR 0x57
#define OLED_I2CAADR 0x3C

#define I2C_SDA 19
#define I2C_SCL 18
#define LED 2


SSD1306AsciiWire oled;

// Code to read the range information from the UltraSonic sensor
unsigned int ping_cm()
{
    unsigned long distance = 0;
    byte i;   
    byte ds[3];

    Wire.beginTransmission(SR04_I2CADDR);
    Wire.write(1);          //1 = cmd to start meansurement
    Wire.endTransmission();
    delay(120);             //1 cycle approx. 100mS. 
    i = 0;
    Wire.requestFrom(0x57,3);  //read distance       
    while (Wire.available())
    {
     ds[i++] = Wire.read();
    }        
    
    distance = (unsigned long)(ds[0]) * 65536;
    distance = distance + (unsigned long)(ds[1]) * 256;
    distance = (distance + (unsigned long)(ds[2])) / 10000;
    
    //measured value between 1cm to 6meters
    if ((1<=distance)&&(600>=distance)) {
        return (int)distance;
    }
    else {
        return 0;
    }
}

void setup() {
    Wire.begin(I2C_SDA, I2C_SCL);    
    oled.begin(&Adafruit128x64, OLED_I2CAADR); 
    pinMode(LED, OUTPUT);
    delay(2000);
    
    Serial.begin(115200);
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("Range Sensor Demo");

}


void loop() {
    oled.clear();
    oled.println("Range <->");
    oled.print("Distance(cm): "); 
    oled.println(ping_cm());
}
                    
                
