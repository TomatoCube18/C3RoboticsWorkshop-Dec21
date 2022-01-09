/* 
 *  Ex_10 - i2C Slave LED
 *  Create an i2C controllable LED on 
 *  the ATtiny1614 Co-processor
 *  
 *    LEDs 
 *    --  PA7 (3)(ATTINY1614) 
 *    
 *    Sound  
 *    --  PA1 (8)(ATTINY1614)
 *    
 *    I2C 
 *    --  IO18 (SCL) <-> PB0 (ATTINY1614  
 *    --  IO19 (SDA) <-> PB1 (ATTINY1614)
 *    
 *    
 *     I2C commands table 
 *     all i2c write commands have 2 bytes parameter (little endian)   
 *     Write:
 *     |   commands         | param1   | param2   |  description                                            |  
 *     |--------------------|----------|----------|---------------------------------------------------------|  
 *     | 0x02 | Enable      | 0/1      | dummy    | PA7 (3) LED.                                            |
 *     
 *    
 */
#include <Wire.h>

// I2C address
#define I2CADDR 0x13

#define LED 3
#define SPK_OUT 8

volatile char rcv_buf[256];
volatile int rcv_len = 0;
uint8_t led_state = 0;

void setup() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    
    pinMode(SPK_OUT, OUTPUT);
    noTone(SPK_OUT);
    
    tone(SPK_OUT, 659, 90); // NOTE_E5
    delay(100);
    tone(SPK_OUT, 587, 90); // NOTE_D5
    delay(100);
    tone(SPK_OUT, 370, 90); // NOTE_FS4
    delay(100);
    noTone(SPK_OUT);
    
    Wire.begin(I2CADDR);            // join i2c bus
    Wire.onReceive(receiveEvent);   // I2C write
    Wire.onRequest(requestEvent);   // I2C read
}

void requestEvent() { // I2C read
    Wire.write(led_state);
}

void receiveEvent(int howMany) {
    int i = 0;
    int p;
    int busy = rcv_len;
    
    while (0 < Wire.available()) { // loop through all but the last
    if (busy == 0) {
            rcv_buf[i++] = Wire.read(); // receive byte as a character
        } else {
            int a = Wire.read(); // dummy read
        }
    }
    if (i == 3) {
        rcv_len = i;
    }
}


void loop() {
    if( rcv_len > 0 ) {
    unsigned int param = ((rcv_buf[2]&0xff)<<8)+(rcv_buf[1]&0xff);  
    // little endian 2bytes param
    
    switch(rcv_buf[0]) {
        case 2: // 0x02 Turn on/off the LED PA1
            if(param & 0x1) {
                digitalWrite(LED, HIGH);
                led_state = HIGH;
            }
            else {
                digitalWrite(LED, LOW);
                led_state = LOW;
            }
            break;
        }
        rcv_len = 0;
    }
}
                    
                
