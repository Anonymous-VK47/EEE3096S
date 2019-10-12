// RTC ONLY !!!

//#ifndef CURRENTTIME_H
//#define CURRENTTIME_H
/*
void getCurrentTime(void);

int getHours(void);
int getMins(void);
int getSecs(void);

//#endif
*/
//*************************************

// #include "CurrentTime.h"
#include <time.h>
#include<iostream>
#include<string> // for string class
using namespace std;

int HH,MM,SS;

void getCurrentTime(void){
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  HH = timeinfo ->tm_hour;
  MM = timeinfo ->tm_min;
  SS = timeinfo ->tm_sec;
}

int getHours(void){
    getCurrentTime();
    return HH;
}

int getMins(void){
    return MM;
}

int getSecs(void){
    return SS;
}

//*************************************

//#ifndef BINCLOCK_H
//#define BINCLOCK_H
/*

// Function definitions
int hFormat(int hours);
int hexCompensation(int units);
int decCompensation(int units);
void init_buttons_and_RTC();
*/

// define constants
const char RTCAddr = 0x6f;
const char SEC = 0x00; // see register table in datasheet
const char MIN = 0x01;
const char HOUR = 0x02;
const char TIMEZONE = 2; // +02H00 (RSA)

const int BTNS[] = {0,2,3,7};

//#endif

//******************************************

//#include <wiringPi.h>
//#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions

//#include "BinClock.h"
//#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance

int HH,MM,SS;

void init_buttons_and_RTC(){
	wiringPiSetup(); // Set up wiringPi

	RTC = wiringPiI2CSetup(RTCAddr); // Set up I2C

	// Set up buttons
	for(int i=0; i < 3; i++){
		    pinMode(BTNS[i], INPUT);
		    pullUpDnControl(BTNS[i], PUD_UP);
	}
	wiringPiISR(BTNS[0],INT_EDGE_RISING, &stop_start_monitoring);
	wiringPiISR(BTNS[1],INT_EDGE_RISING, &dismiss_alarm);
	wiringPiISR(BTNS[2],INT_EDGE_RISING, &reset_system_time);
	wiringPiISR(BTNS[3],INT_EDGE_RISING, &change_reading_interval);
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




int main(void){
	init_buttons_and_RTC();

	wiringPiI2CWriteReg8(RTC, HOUR, 0x0+TIMEZONE);
	wiringPiI2CWriteReg8(RTC, MIN, 0x0);
	wiringPiI2CWriteReg8(RTC, SEC, 0x80);

	// Repeat this until we shut down
	for (;;){
		//System time
		hours = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		mins = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
		secs = hexCompensation(wiringPiI2CReadReg8(RTC, SEC)-0x80);

		//RTC time
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		string System_time = to_string(hours)+to_string(mins)+to_string(secs);
		string RTC_time = to_string(HH)+to_string(MM)+to_string(SS);

		// Print out the time we have stored on our RTC
		printf("-----------------------------------\n");
		printf("|"+" RTC Time "+"|"+" Sys Time "+"|\n");
		printf("| "+RTC_time+" | "+System_time+" |\n");

		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}
