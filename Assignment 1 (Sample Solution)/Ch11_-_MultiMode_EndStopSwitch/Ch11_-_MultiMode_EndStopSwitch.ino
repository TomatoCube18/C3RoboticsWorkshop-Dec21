/* 
 *  Ex_06 - Physical Computing
 *  Revisit Input & Output
 *  
 *  LED 
 *  -- IO2
 *  Switch
 *  -- IO9
 *  
 */

#include <Wire.h>

#define I2C_SDA 19
#define I2C_SCL 18
#define PEXP_I2CADDR 0x23

#define ONBOARD_LED 2    // LED IO2
#define ONBOARD_SW  9    // Switch IO9
#define LONGPRESSED_MILLI 1500  //1.5 seconds

int current_LED = LOW;
byte current_mode = 0;
byte current_ButtonState = HIGH;
byte previous_ButtonState = HIGH;
long lastButton_Activity = 0;

uint8_t expanderData;

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
   Serial.begin(115200); //set up serial library baud rate to 115200
   Wire.begin(I2C_SDA, I2C_SCL);  
   
   pinMode(ONBOARD_LED, OUTPUT);
   pinMode(ONBOARD_SW, INPUT);

   digitalWrite(ONBOARD_LED, current_LED);    // turn the LED off by making the voltage LOW   

   IOexpanderWrite(PEXP_I2CADDR, 0xff);
}

void loop() {
    expanderData = IOexpanderRead(PEXP_I2CADDR);

    // Button Reading portion
    // current_ButtonState = digitalRead(ONBOARD_SW);

/*
    // Utilize serial print to identify which button is 5, 6 and 7
    Serial.print("EndStop SW #1: ");    
    Serial.println(bitRead(expanderData, 5), BIN);
    
    Serial.print("EndStop SW #2: ");      
    Serial.println(bitRead(expanderData, 6), BIN);  
    
    Serial.print("EndStop SW #3: ");    
    Serial.println(bitRead(expanderData, 7), BIN);

    delay(100);
*/    
    
    current_ButtonState = bitRead(expanderData, 7);    

    
    // Transition from High to Low, Button press instance
    if ((current_ButtonState == LOW) && (previous_ButtonState == HIGH)){
        lastButton_Activity = millis();
        Serial.println("State Change");
    }
    else if ((lastButton_Activity > 0) && (millis() >= lastButton_Activity + LONGPRESSED_MILLI)) {
        Serial.println("Long Press detected");
        lastButton_Activity = -1;  // Disable Long press mechanism 

        // Toggle LED
        current_LED = (current_LED == HIGH) ? LOW: HIGH;
        digitalWrite(ONBOARD_LED, current_LED);
    }
    else if (current_ButtonState == HIGH) {   // Release button before long press timeout
        lastButton_Activity = -1;  // Disable Long press mechanism
    }
    previous_ButtonState = current_ButtonState;
    

}
                    
                
