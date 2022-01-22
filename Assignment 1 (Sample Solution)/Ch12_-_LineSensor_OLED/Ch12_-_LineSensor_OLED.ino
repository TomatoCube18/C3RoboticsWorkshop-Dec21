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

uint8_t expanderData;
byte switch_1_state = HIGH;
byte switch_2_state = HIGH;
byte switch_3_state = HIGH;

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIXEL, NEO_GRB + NEO_KHZ800);
SSD1306AsciiWire oled;


//Write a byte to the IO expander
void IOexpanderWrite(byte address, byte _data) {
    Wire.beginTransmission(address);
    Wire.write(_data);
    Wire.endTransmission(); 
}

//Read a byte from the IO expander
uint8_t IOexpanderRead(int address) {
    uint8_t _data;
    Wire.requestFrom(address, 1);
    if(Wire.available()) {
        _data = Wire.read();
    }
    return _data;
}


void setup() {
    Wire.begin(I2C_SDA, I2C_SCL);   
    oled.begin(&Adafruit128x64, OLED_I2CAADR); 
 
    Serial.begin(115200); //set up serial library baud rate to 115200
    Serial.println("IO Port Expander Test");

    // Initialize PCF8574 configuration -> Pull High to Read
    IOexpanderWrite(PEXP_I2CADDR, 0xff);
    
    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(20); // Set BRIGHTNESS to about 1/5 (max = 255)
    randomSeed(analogRead(0));
  
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("Hello NUS!");
    oled.println("Line Module: ");     
    
    delay(500);
}

void loop() {
  
    expanderData = IOexpanderRead(PEXP_I2CADDR);

    switch_1_state = bitRead(expanderData, 5);
    switch_2_state = bitRead(expanderData, 6);
    switch_3_state = bitRead(expanderData, 7);    
    
    strip.setPixelColor(0, strip.Color((switch_1_state == LOW)? 255: 0,0,0));
    strip.setPixelColor(1, strip.Color(0,(switch_2_state == LOW)? 255: 0,0));
    strip.setPixelColor(2, strip.Color(0,0,(switch_3_state == LOW)? 255: 0));
    strip.show(); // This sends the updated pixel color to the hardware.

    Serial.println(expanderData & 0x1F , BIN);  // Disregard the first 3 bits
     
    oled.setCursor(0, 2);
    oled.print(expanderData & 0x1F , BIN); oled.println("      ");  // Disregard the first 3 bits
    delay(500);
    
}
                    
                
