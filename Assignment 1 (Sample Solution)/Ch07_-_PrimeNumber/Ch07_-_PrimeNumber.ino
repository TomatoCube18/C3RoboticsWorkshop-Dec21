/* 
 *  Ex_04 - Loops
 *  Prime Number
 *  
 */

int i = 1;
int c = 0;
int count = 1;
int n = 30; // to generate the 1st n prime number in the number system

void setup() {
   Serial.begin(115200); //set up serial library baud rate to 115200
   delay(1000);

    // iteration for n prime numbers
    // i is the number to be checked in each iteration starting from 1
    while(count <= n)   
    {
        // iteration to check if i is prime or not
        for(c = 2; c < i; c++)
        {
            if(i%c == 0) //the moment i can be divided by c, break the loop.
                break;
        }

        if(c == i)  // c is prime
        {
            Serial.print(count);            
            Serial.print(":");
            Serial.println(i);
            count++;    // increment the count of prime numbers
        }
      i++;
    }
    
}

void loop() { // <- Loop() is a forever while loop in disguise.

   
}
