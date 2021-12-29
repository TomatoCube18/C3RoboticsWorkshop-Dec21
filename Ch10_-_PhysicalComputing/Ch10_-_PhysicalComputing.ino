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

int current_LED = LOW;

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   pinMode(ONBOARD_LED, OUTPUT);
   pinMode(ONBOARD_SW, INPUT);

   digitalWrite(ONBOARD_LED, current_LED);    // turn the LED off by making the voltage LOW   
}

void loop() {
   current_LED = (current_LED == HIGH) ? LOW: HIGH;
   digitalWrite(ONBOARD_LED, current_LED);
   
   if (digitalRead(ONBOARD_SW) == LOW) {  // Switch is asserted Low (Active Low)
      delay(100);
   }
   else {
      delay(500);
   }
}
