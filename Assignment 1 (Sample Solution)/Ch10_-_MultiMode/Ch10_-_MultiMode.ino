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
byte current_mode = 0;
byte current_ButtonState = HIGH;
byte previous_ButtonState = HIGH;

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   pinMode(ONBOARD_LED, OUTPUT);
   pinMode(ONBOARD_SW, INPUT);

   digitalWrite(ONBOARD_LED, current_LED);    // turn the LED off by making the voltage LOW   
}

void loop() {

    // Button Reading portion
    current_ButtonState = digitalRead(ONBOARD_SW);
    
    // Transition from High to Low, Button press instance
    if ((current_ButtonState == LOW) && (previous_ButtonState == HIGH)){
        current_mode = (current_mode >= 3)? 0 : current_mode + 1;
        Serial.print("Button Mode ");
        Serial.println(current_mode);
    }
    previous_ButtonState = current_ButtonState;
    
   if (current_mode == 0) {  // Switch is asserted Low (Active Low)           
     current_LED = ((millis() / 1000) % 2 == 0)? LOW: HIGH; 
   }
   else if (current_mode == 1) {       
     current_LED = ((millis() / 500) % 2 == 0)? LOW: HIGH; 
   }
   else if (current_mode == 2) {   
     current_LED = ((millis() / 100) % 2 == 0)? LOW: HIGH; 
   }
   else {
      current_LED = LOW;
   }
   
   digitalWrite(ONBOARD_LED, current_LED);
}
                    
                
