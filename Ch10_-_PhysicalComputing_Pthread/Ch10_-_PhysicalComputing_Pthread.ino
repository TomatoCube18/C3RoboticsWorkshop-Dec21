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

#define ONBOARD_LED 2    // LED IO2
#define ONBOARD_SW  9    // Switch IO9

#include <pthread.h>

static byte current_LED = LOW;
static byte current_mode = 0;
static byte current_ButtonState = HIGH;
static byte previous_ButtonState = HIGH;

pthread_t threads[2]; // We are creating 2 threads

void * blinkFunc(void * arg) {
    Serial.println("LED Thread created.");
    while(1) {
        current_LED = (current_mode == 0)? LOW : (current_LED == HIGH) ? LOW: HIGH;
        digitalWrite(ONBOARD_LED, current_LED);
        switch(current_mode) {
          case 1:
              delay(1000);
          break;
          case 2:
              delay(500);
          break;
          case 3:
              delay(100);
          break;
          default:
              delay(2);
        }
    }
    return NULL;  
}

void * buttonFunc(void * arg) {
    Serial.println("Button Thread created.");
    while (1) {
      current_ButtonState = digitalRead(ONBOARD_SW);
      
      // Transition from High to Low, Button press instance
      if ((current_ButtonState == LOW) && (previous_ButtonState == HIGH)){
          current_mode = (current_mode >= 3)? 0 : current_mode + 1;
          Serial.print("Button Mode ");
          Serial.println(current_mode);
      }
      previous_ButtonState = current_ButtonState;
      delay(2);
    }
    return NULL;
}

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   pinMode(ONBOARD_LED, OUTPUT);
   pinMode(ONBOARD_SW, INPUT);

   digitalWrite(ONBOARD_LED, current_LED);    // turn the LED off by making the voltage LOW   
   delay(1000);
   
   pthread_create(&threads[0], NULL, &buttonFunc, NULL);
   pthread_create(&threads[1], NULL, &blinkFunc, NULL);
}

void loop() {
   
}
