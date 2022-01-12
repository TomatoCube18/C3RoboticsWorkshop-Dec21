/* 
 *  Ex_03 - Functions
 *  Implement a program that could encrypt & decrypt a message with the CoreModule.
 *  Encryption logic is implemented similar to: https://www.boxentriq.com/code-breaking/caesar-cipher
 *  
 * 
 */


String message;
char encodedChar;
int myKey = 2;

char encryptChar(char input, int key)
{
  char value = (int)input; //converting character to ascii value

  //encryting upper case character
  if (value >= 65 && value <= 90) 
  {
    value = value + key;
    
    if(value > 90) //if character is beyond Z, reposition it from A
      value = 64 + (value - 90);
  }
  
  //encryting lower case character
  else if (value >= 97 && value <= 122)
  {
    value = value + key;
    
    if(value > 122) //if character is beyond z, reposition it from a
      value = 96 + (value - 122);  
  }
  
  return (char)value;   
}


void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   delay(1000);
   Serial.print("Insert the message that you'd like to encrypt: ");
}


void loop() {
   if(Serial.available()) { //if number of bytes (characters) available for reading
      message = Serial.readString();      
      Serial.println(message);

      Serial.print("Encrypted Message: ");
      
      for(int i = 0; i < message.length(); i++) {
        encodedChar = encryptChar(message[i],myKey); 
        Serial.print(encodedChar);
      }
   }
}
