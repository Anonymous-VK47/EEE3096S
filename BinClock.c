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
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions

#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

int HH,MM,SS;
float count = 0.0;

int wiringPiISR (int pin, int edgeType){
      if (pin==5){
           minInc();}
      else{ hourInc();}
     };
int softPwmCreate (int pin, int initialValue, int pwmRange);
void softPwmWrite (int pin, int value);

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
        //pinMode(SECS, OUTPUT);
        //pullUpDnControl(SECS, PUD_UP);
	//softPWMCreate(SECS, (100.0/6.0), (1.0/60.0));
	
	printf("LEDS done\n");
	
	//Set up the Buttons
	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[j], INPUT);
		pullUpDnControl(BTNS[j], PUD_UP);
	}
	
	//Attach interrupts to Buttons
	int mm = wiringPiISR(BTNS[0], INT_EDGE_RISING);
        //int hh = wiringPiISR(BTNS[1], INT_EDGE_RISING, &hourInc());

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
	//wiringPiI2CWriteReg8(RTC, HOUR, 0x18+decCompensation(TIMEZONE));
	//wiringPiI2CWriteReg8(RTC, MIN, 0x4);
	wiringPiI2CWriteReg8(RTC, SEC, 0x80);
	
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
                hours = wiringPiI2CReadReg8(RTC, HOUR);
                mins = wiringPiI2CReadReg8(RTC, MIN);
                secs = wiringPiI2CReadReg8(RTC, SEC)-0x80;
		
		//Function calls to toggle LEDs
		//lightHours(hours);
                //lightMins(mins);
                //secPWM(secs);
		
		// Print out the time we have stored on our RTC
		printf("The current time is: %d:%d:%d\n", hours, mins, secs);

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
	//int hrs = hexCompensation(units);
        //if (hrs>=24){
        //       int new_hrs = hrs-24;
        //int Hrs = hFormat(new_hrs);
        //char b = ("%b",Hrs);

	
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(int units){
	//int mns = hexCompensation(units);
        //if (mns>=60){
        //       int new_mns = mns-60;
}

/*
 * PWM on the Seconds LED
 * The LED should have 60 brightness levels
 * The LED should be "off" at 0 seconds, and fully bright at 59 seconds
 */
void secPWM(int units){
	//softPwmWrite(SECS, (100.0/60.0)*units);
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
                //int h = wiringPiI2CReadReg8(RTC, HOUR);

		//Increase hours by 1, ensuring not to overflow
                //int H = hexCompensation(h);
               // if (H>=24){
                        //int h2 = (H-24)+1;}
                //else { h2 = H+1; }

		//Write hours back to the RTC
                //wiringPiI2CWriteReg8(RTC, h2);


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
                //int m = wiringPiI2CReadReg8(RTC, MIN);

		//Increase minutes by 1, ensuring not to overflow
                //int M = hexCompensation(m);
               // if (M>=60){
                        //int m2 = (M-60)+1;}
                //else { m2 = M+1;}

		//Write minutes back to the RTC
                //wiringPiI2CWriteReg8(RTC, m2);

	}
	lastInterruptTime = interruptTime;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		HH = hFormat(HH);
		HH = decCompensation(HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}
