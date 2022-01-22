/* 
 *  Ex_11 - Misc i2C Range Sensor Exercise
 *  Retrieve Range information from Ultrasonic & VL6180X ToF sensor
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
#include "Adafruit_VL6180X.h"
 
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

Adafruit_VL6180X vl = Adafruit_VL6180X();


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
//    delay(200);

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
    oled.println("Range Sensor Demo");

    Serial.println("Adafruit VL6180x test!");
    if (! vl.begin()) {
      Serial.println("Failed to find sensor");
      while (1);
    }
  
}


void loop() {
    oled.setCursor(0, 0);
    oled.println("<-> Ultrasonic <->");
    oled.print("Distance(mm): "); 
    oled.print(ping_mm()); oled.println("     ");


    oled.println(""); 
    oled.println("<-> ToF <->");
    float lux = vl.readLux(VL6180X_ALS_GAIN_5);
    oled.print("Lux: "); oled.print(lux); oled.println("     ");
  
    uint8_t range = vl.readRange();
    uint8_t status = vl.readRangeStatus();

    if (status == VL6180X_ERROR_NONE) {
      oled.print("Range(mm): "); oled.print(range); oled.println("     ");
    }

    // Some error occurred, print it out!
    
    if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
      Serial.println("System error");
    }
    else if (status == VL6180X_ERROR_ECEFAIL) {
      Serial.println("ECE failure");
    }
    else if (status == VL6180X_ERROR_NOCONVERGE) {
      Serial.println("No convergence");
    }
    else if (status == VL6180X_ERROR_RANGEIGNORE) {
      Serial.println("Ignoring range");
    }
    else if (status == VL6180X_ERROR_SNR) {
      Serial.println("Signal/Noise error");
    }
    else if (status == VL6180X_ERROR_RAWUFLOW) {
      Serial.println("Raw reading underflow");
    }
    else if (status == VL6180X_ERROR_RAWOFLOW) {
      Serial.println("Raw reading overflow");
    }
    else if (status == VL6180X_ERROR_RANGEUFLOW) {
      Serial.println("Range reading underflow");
    }
    else if (status == VL6180X_ERROR_RANGEOFLOW) {
      Serial.println("Range reading overflow");
    }
    
    delay(50);

    
}
                    
                
