/* 
 *  Ex_03 - Conditional
 *  Simple communication through the Serial monitor.
 *  
 */
String rcvString = "";
int ghost = 0;

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   delay(1000);
   Serial.println("How many ghost do you see?");
}

void loop() {
   if(Serial.available()) { //if number of bytes (characters) available for reading
      Serial.print("You saw "); //print I received
      rcvString = Serial.readString();
      ghost = rcvString.toInt(); // read string until newline character
      Serial.print(ghost); 
      Serial.println(" ghosts!");

      if (ghost > 1) {
        Serial.println("It's so spoooooky!");
      }
      else if (ghost == 1) {
        Serial.println("Hello Casper!!");
      }
      else {
        Serial.println("Nobody is here...");
      }

      Serial.println("");
      Serial.println("How many ghost do you see?");
   }
}
