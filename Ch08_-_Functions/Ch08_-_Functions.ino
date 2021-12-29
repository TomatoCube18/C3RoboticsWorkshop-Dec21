/* 
 *  Ex_03 - Functions
 *  A stand-alone morse code encoder which takes a 
 *  Serial character input & convert it to morse code.
 *  
 *  LED 
 *  -- IO2
 *  
 */

#define ONBOARD_LED 2    // LED IO2
#define SHORT_DELAY 300
#define LONG_DELAY 900

char input;

void dot() {
  digitalWrite(ONBOARD_LED,HIGH); 
  delay (SHORT_DELAY); 
  digitalWrite(ONBOARD_LED,LOW); 
  delay (SHORT_DELAY);
}

void dash() {
  digitalWrite(ONBOARD_LED,HIGH); 
  delay (LONG_DELAY); 
  digitalWrite(ONBOARD_LED,LOW); 
  delay (SHORT_DELAY);
}

void shortspace() {  //delay between letters
  digitalWrite(ONBOARD_LED,LOW); 
  delay(SHORT_DELAY);
  delay(SHORT_DELAY);
}

void space() { //space bar
  digitalWrite(ONBOARD_LED,LOW); 
  delay(LONG_DELAY);
  delay(SHORT_DELAY);
}

void morseCharacter(char input) {
  char _input = input;
  _input = ((_input >= 'a') && (_input <= 'z')) ? _input - 32 : _input; //lower to upper case
  
  switch (_input) {
    case 'A':
      {dot();dash();shortspace();}break;  // A in morseCode
    case 'B':
      {dash();dot();dot();dot();shortspace();}break;
    case 'C':
      {dash();dot();dash();dot();shortspace();}break;
    case 'D':
      {dash();dot();dot();shortspace();}break;
    case 'E':
      {dot();shortspace();}break;
    case 'F':
      {dot();dot();dash();dot();shortspace();}break;
    case 'G':
      {dash();dash();dot();shortspace();}break;
    case 'H':
      {dot();dot();dot();dot();shortspace();}break;
    case 'I':
      {dot();dot();shortspace();}break;
    case 'J':
      {dot();dash();dash();dash();shortspace();} break;
    case 'K':
      {dash();dot();dash();shortspace();}
      break;
    case 'L':
      {dot();dash();dot();dot();shortspace();}break;
    case 'M':
      {dash();dash();shortspace();}break;
    case 'N':
      {dash();dot();shortspace();}break;
    case 'O':
      {dash();dash();dash();shortspace();}break;
    case 'P':
      {dot();dash();dash();dot();shortspace();}break;
    case 'Q':
      {dash();dash();dot();dash();shortspace();}break;
    case 'R':
      {dot();dash();dot();shortspace();}break;
    case 'S':
      {dot();dot();dot();shortspace();}break;
    case 'T':
      {dash();shortspace();}break;
    case 'U':
      {dot();dot();dash();shortspace();}break;
    case 'V':
      {dot();dot();dot();dash();shortspace();}break;
    case 'W':
      {dot();dash();dash();shortspace();}break;
    case 'X':
      {dash();dot();dot();dash();shortspace();}break;
    case 'Y':
      {dash();dot();dash();dash();shortspace();}break;
    case 'Z':
      {dash();dash();dot();dot();shortspace();}break;
    case '1':
      {dot();dash();dash();dash();dash();shortspace();}break; //1 in morseCode
    case '2':
      {dot();dot();dash();dash();dash();shortspace();}break;
    case '3':
      {dot();dot();dot();dash();dash();shortspace();}break;
    case '4':
      {dot();dot();dot();dot();dash();shortspace();}break;
    case '5':
      {dot();dot();dot();dot();dot();shortspace();}break;
    case '6':
      {dash();dot();dot();dot();dot();shortspace();}break;
    case '7':
      {dash();dash();dot();dot();dot();shortspace();}break;
    case '8':
      {dash();dash();dash();dot();dot();shortspace();}break;
    case '9':
      {dash();dash();dash();dash();dot();shortspace();}break;
    case '0':
      {dash();dash();dash();dash();dash();shortspace();}break;
    case ' ':
      space();break;
    default:
      shortspace();break;
  }
}
void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   pinMode(ONBOARD_LED, OUTPUT);
   delay(1000);
   Serial.println("MorseCode encoder");
}

void loop() {
   if(Serial.available()) { //if number of bytes (characters) available for reading
      input = Serial.read();
      morseCharacter(input);
      Serial.println (input);
   }
}
