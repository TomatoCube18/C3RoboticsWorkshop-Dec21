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
#define LONGPRESSED_MILLI 1500  //1.5 seconds

int current_LED = LOW;
byte current_mode = 0;
byte current_ButtonState = HIGH;
byte previous_ButtonState = HIGH;
long lastButton_Activity = 0;

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
        lastButton_Activity = millis();
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
                    
                
