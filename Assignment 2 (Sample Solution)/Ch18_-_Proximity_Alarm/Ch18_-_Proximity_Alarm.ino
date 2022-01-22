/* 
 *  Ex_11 - Misc i2C Range Sensor
 *  Retrieve Range information from the i2C Ultrasonic Sensor
 *  Proximity Alarm exercise
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
 
#include <esp_task_wdt.h>

#include "pitches.h"
#include <Adafruit_NeoPixel.h>

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// I2C address
#define SR04_I2CADDR 0x57
#define OLED_I2CAADR 0x3C

// i2c Slave Co-processor - On the Core-Module
#define I2CADDR 0x13

#define I2C_SDA 19
#define I2C_SCL 18
#define LED 2

#define NEO_PIXEL 5
#define LED_COUNT 3

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIXEL, NEO_GRB + NEO_KHZ800);
SSD1306AsciiWire oled;

byte attinySlaveArray[3];
unsigned int safeDistance = 100; // mm
int currentDistance = 200; // mm


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


// Code to read the range information from the UltraSonic sensor
int ping_mm()
{
    unsigned long distance = 0;
    byte i;   
    byte ds[3];
    long timeChecked;
    
    Wire.beginTransmission(SR04_I2CADDR);
    Wire.write(1);          //1 = cmd to start measurement.
    Wire.endTransmission();

    vTaskDelay(pdMS_TO_TICKS(200)); // Delay 200ms

    i = 0;
    Wire.requestFrom(0x57,3);  //read distance       
    while (Wire.available())
    {
     ds[i++] = Wire.read();
    }        
    
    distance = (unsigned long)(ds[0] << 16);
    distance = distance + (unsigned long)(ds[1] << 8);
    distance = (distance + (unsigned long)(ds[2])) / 1000;
    //measured value between 1cm to 6meters
    if ((10 <= distance) && (6000 >= distance)) {
        return (int)distance;
    }
    else {
        return -1;
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
    oled.println("Proximity Sentry Alarm!");

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    randomSeed(analogRead(0));
    
}


void loop() {

  //  while((millis() / 500) % 2 == 0){ // every 0.5 second, check and display distance
          
        currentDistance = ping_mm();
        
        if (currentDistance > 0) { 
              oled.clear();
              oled.println("Range <->");
              oled.print("Distance(mm): "); 
              oled.println(currentDistance);    
          
              if(currentDistance < safeDistance) {
                  oled.println("");
                  oled.println("Danger ahead!");
                  
                  strip.setPixelColor(0, strip.Color(random(255),0,0));  // warning RED!
                  strip.setPixelColor(1, strip.Color(random(255),0,0));  // warning RED!
                  strip.setPixelColor(2, strip.Color(random(255),0,0));  // warning RED!
                  strip.show(); // This sends the updated pixel color to the hardware.
              
                  playNote(NOTE_A4, 100);
                  playNote(NOTE_A4, 100);
                  playNote(NOTE_C4, 100);           
          
              }
              else {          
                  strip.setPixelColor(0, strip.Color(0,0, (currentDistance < (safeDistance * 1.5)) ? 255 : 0 ));
                  strip.setPixelColor(1, strip.Color(0,0, (currentDistance < (safeDistance * 3)) ? 255 : 0 ));
                  strip.setPixelColor(2, strip.Color(0,0, (currentDistance < (safeDistance * 6)) ? 255 : 0 ));
                  strip.show(); // This sends the updated pixel color to the hardware.
              }          
        } 
        else {
            oled.clear();
            oled.println("Range <->");
            oled.print("Out-of-Range!!");
            
            strip.setPixelColor(0, strip.Color(0,0,0));    
            strip.setPixelColor(1, strip.Color(0,0,0));       
            strip.setPixelColor(2, strip.Color(0,0,0));       
            strip.show(); // This sends the updated pixel color to the hardware.
        }
}
                    
                
