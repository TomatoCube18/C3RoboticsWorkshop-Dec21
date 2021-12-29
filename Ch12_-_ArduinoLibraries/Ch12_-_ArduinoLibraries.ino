/* 
 *  Ex_08 - Arduino Libraries
 *  Printing text to OLED Display
 *  Random color display on Neopixels
 *  
 *  PortExpander
 *  -- Bit 4 - 0 
 *  -- Line sensor Module
 *  -- Bit 7 - 5
 *  -- End Stop switch Module
 *  
 *  NeoPixel
 *  -- IO5
 */
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_SDA 19
#define I2C_SCL 18

#define PEXP_I2CADDR 0x23
#define OLED_I2CAADR 0x3C

#define NEO_PIXEL 5
#define LED_COUNT 3

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIXEL, NEO_RGB + NEO_KHZ800);
SSD1306AsciiWire oled;

void setup() {
    Wire.begin(I2C_SDA, I2C_SCL);   
    oled.begin(&Adafruit128x64, OLED_I2CAADR); 
 
    Serial.begin(115200); //set up serial library baud rate to 115200
    Serial.println("IO Port Expander Test");
    
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    randomSeed(analogRead(0));
  
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("Hello NUS!");
}

void loop() {
    strip.setPixelColor(0, strip.Color(random(255),random(255),random(255)));
    strip.setPixelColor(1, strip.Color(random(255),random(255),random(255)));
    strip.setPixelColor(2, strip.Color(random(255),random(255),random(255)));
    strip.show(); // This sends the updated pixel color to the hardware.
  
    delay(500);
}
