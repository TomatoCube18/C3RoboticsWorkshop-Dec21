/* 
 *  Ex_06 - i2c PortExpander
 *  Getting sensor reading from EndStop Switch &
 *  5 channel lineSensors
 *  
 *  PortExpander
 *  -- Bit 4 - 0 
 *  -- Line sensor Module
 *  -- Bit 7 - 5
 *  -- End Stop switch Module
 */
#include <Wire.h>

#define I2C_SDA 19
#define I2C_SCL 18

#define PEXP_I2CADDR 0x23

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
    Wire.begin(I2C_SDA, I2C_SCL);    
    Serial.begin(115200); //set up serial library baud rate to 115200
    Serial.println("IO Port Expander Test");
    
    // Initialize PCF8574 configuration -> Pull High to Read
    IOexpanderWrite(PEXP_I2CADDR, 0xff);
}

void loop() {
    expanderData = IOexpanderRead(PEXP_I2CADDR);
    Serial.print("EndStop SW: ");
    Serial.print(expanderData >> 5, BIN);
    Serial.print("Line Module: ");
    Serial.println(expanderData & 0x1F , BIN);
    
    delay(500);
}
