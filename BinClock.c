/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <AGSVAC001> <STRDAV029>
 * 16 August 2019
*/

#include <wiringPi.h>
#include <softPwm.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions
#include <math.h>
#include <stdbool.h>

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

int HH,MM,SS;
int copy_of_bits;
float count = 0.0;

int wiringPiISR (int pin, int edgeType,void (*function)(void));
int softPwmCreate (int pin, int initialValue, int pwmRange);
void softPwmWrite (int pin, int value);
char hours1[4]= {4,3,2,0};
char mins1[6] = {27,25,22,21,7,6};
int overflow=0;

char *decToBin(int decValue){
     //Converts decimal numbers to binary
      int quo = 0,rem = 0;
      int bits = log(decValue)/log(2);
      bits++;
      copy_of_bits=bits;
      char *bin = malloc(sizeof(char)*(bits));
      for(int k=0; k<6;k++){
             quo = decValue/2;
             rem = decValue%2;
             decValue = quo;
             bin[bits-1]= rem;
             bits--;
             }
      return bin;
      }

void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. If you want to change pinouts, be aware
	
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
	    pinMode(LEDS[i], OUTPUT);
	}
	
	//Set Up the Seconds LED for PWM
        pinMode(1, OUTPUT);
        pullUpDnControl(1, PUD_UP);
        int xs = softPwmCreate(1, 0, 59);
	printf("LEDS done\n");
	
	//Set up the Buttons
	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);
	}
	
	//Attach interrupts to Buttons
        void interrupt();
	wiringPiISR(5, INT_EDGE_RISING, &minInc);
        wiringPiISR(30, INT_EDGE_RISING, &hourInc);

	printf("BTNS done\n");
	printf("Setup done\n");
}


/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	initGPIO();

	//Set random time (3:04PM)
	//You can comment this file out later
	//wiringPiI2CWriteReg8(RTC, HOUR,0); //0x1+TIMEZONE);
	//wiringPiI2CWriteReg8(RTC, MIN, 0x0);
	//wiringPiI2CWriteReg8(RTC, SEC, 0x80);
	toggleTime();
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
                hours = wiringPiI2CReadReg8(RTC, HOUR);
                mins = wiringPiI2CReadReg8(RTC, MIN);
                secs = wiringPiI2CReadReg8(RTC, SEC)-0x80;
		
		//Function calls to toggle LEDs
		lightHours(hFormat(hexCompensation(hours)));
                lightMins(hexCompensation(mins));
                secPWM(hexCompensation(secs));

		////////////
		// Print out the time we have stored on our RTC0
		printf("The current time is: %d:%d:%d\n", hFormat(hexCompensation(hours)), hexCompensation(mins),hexCompensation(secs));
                printf("hours %d", hours);
		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
                //count ++;
                
	}
	return 0;
}

/*
 * Change the hour format to 12 hours
 */
int hFormat(int hours){
	/*formats to 12h*/
	if (hours >= 24){
		hours = 0;
	}
	else if (hours > 12){
		hours -= 12;
	}
	return (int)hours;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(int units){
       // if (units>=24){
         //      units = hFormat(units-24);}
       // else{ units = hFormat(units);}
       char *b = decToBin(units);
       int j = 0;
        int i = 0;
        //bool x = false;
        printf("copy of bits: %d",copy_of_bits);
        if ((4-copy_of_bits)>0){
           for(i = 0; i<(4-copy_of_bits); i++){
               digitalWrite(hours1[i], 0);
               }
           }
    // int x;
    // int counter=0;
     //int n =3;
    // for(n; n>=0;n--){
      //   x = units >> n;
        // if (x&1){
          //    digitalWrite(hours1[counter],1);
             //}

        //else{ 
          //    digitalWrite(hours1[counter], 0);
         // }
        // counter++;}

         
       for(j; j<copy_of_bits;j++){
            digitalWrite(hours1[i], b[j]);
            i++;
      }
     
            for(j;j<copy_of_bits; j++){
             digitalWrite(mins1[i], b[j]);
             i++;
       }
  }

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
       // if (units>=60){
         //      units = units-60;}
       char *b = decToBin(units);
       int i = 0;
       int j = 0;
     //int x;
      //int counter=0;
      //int n =5;
      //for(n; n>=0;n--){
        //  x = units >> n;
          //if (x&1){
            //  digitalWrite(mins1[counter],1);
             //}

         //else{ 
           //   digitalWrite(mins1[counter], 0);
          //}
         //counter++;}
       if ((6-copy_of_bits)>0){
           for(i = 0; i<(6-copy_of_bits); i++){
             digitalWrite(mins1[i], 0);
               }
            }
        for(j;j<copy_of_bits; j++){
             digitalWrite(mins1[i], b[j]);
             i++;
        }
        
}

/*
 * PWM on the Seconds LED
 * The LED should have 60 brightness levels
 * The LED should be "off" at 0 seconds, and fully bright at 59 seconds
 */
void secPWM(int units){
        int PWM_value = (int)(units);
        softPwmWrite(1, PWM_value);
}

/*
 * hexCompensation
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}


/*
 * hourInc
 * Fetch the hour value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 23 hours in a day
 * Software Debouncing should be used
 */
void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 1 triggered, %x\n", hours);
		//Fetch RTC Time
                int h = wiringPiI2CReadReg8(RTC, HOUR);
                int h2;
		//Increase hours by 1, ensuring not to overflow
                int H = hexCompensation(h);
                printf("H: %d", H);
                if (H>=23){
                        h2=0;}
                else { h2 = H+1; }
                printf("h2: %d",h2);

		//Write hours back to the RTC
                wiringPiI2CWriteReg8(RTC,HOUR, decCompensation(h2));


	}
	lastInterruptTime = interruptTime;
}

/* 
 * minInc
 * Fetch the minute value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 60 minutes in an hour
 * Software Debouncing should be used
 */
void minInc(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		printf("Interrupt 2 triggered, %x\n", mins);
		//Fetch RTC Time
                int m = wiringPiI2CReadReg8(RTC, MIN);
                int m2 ;

		//Increase minutes by 1, ensuring not to overflow
                int M = hexCompensation(m);
                if (M>=59){
                        m2 = 0;}
                else { m2 = M+1;}

		//Write minutes back to the RTC
                wiringPiI2CWriteReg8(RTC, MIN, decCompensation(m2));

	}
	lastInterruptTime = interruptTime;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
                printf("hours: %d",HH);
		MM = getMins();
		SS = getSecs();

		HH = decCompensation(HH);
                printf("HH comp %d", HH);
                HH = hFormat(HH);
                printf("HH form: %d",HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}
