/* 
 *  Ex_04 - Loops
 *  Meet the FlintStones
 *  
 */

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   delay(1000);

   Serial.print("When you're with the Flintstones. Have");
   for (int count = 0; count < 5; count ++) {
      Serial.print(" A yabba dabba doo time");
   }
   Serial.println(".");

   String flintStoneS[4] = {"Fred", "Wilma", "Pebbles", "Dino"};
   for (int flntStoneCnt = 0; flntStoneCnt <= 3; flntStoneCnt ++) {
      Serial.println("I am a Flintstone. My name is " + flintStoneS[flntStoneCnt]);
   }

}

void loop() { // <- Loop() is a forever while loop in disguise.
   
}
