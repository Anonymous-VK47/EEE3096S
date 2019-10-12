// ADC ONLY !!!

//#ifndef PRAC4_H
//#define PRAC4_H

//Includes
#include <wiringPi.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

void mcp3008_read(uint8_t);
/*
#define BUFFER_SIZE 1000

//SPI Settings
#define SPI_CHAN 1// Write your value here
#define SPI_SPEED 409600// Write your value here


//Function definitions
int setup_gpio(void);
int main(void);

//#endif

*/
//******************************************/

//#include "Prac4.h"

using namespace std;
/*
unsigned char* buffer[3][2];
unsigned char ch0[4]=0b1000;
unsigned char ch1[4]=0b1001;
unsigned char ch2[4]=0b1011;
/*
unsigned char buffer1[2][BUFFER_SIZE][2];
int buffer_location = 0;
bool bufferReading = 0; //using this to switch between column 0 and 1 - the first column

bool threadReady = false; //using this to finish writing the first column at the start of the song, before the column is played


int setup_gpio(void){
    //Set up wiring Pi
    wiringPiSetup();
    //setting up the buttons
	//TODO

    //setting up the SPI interface
    wiringPiSPISetup(1,409600); // Set up SPI

    return 0;
}

/*
 * Thread that handles writing to SPI
 *
 * You must pause writing to SPI if not playing is true (the player is paused)
 * When calling the function to write to SPI, take note of the last argument.
 * You don't need to use the returned value from the wiring pi SPI function
 * You need to use the buffer_location variable to check when you need to switch buffers

void *playThread(void *threadargs){
    // If the thread isn't ready, don't do anything
    while(threadReady)
        continue;
        
        //buffer[0][0]= startADC;
	//Write the buffer out to SPI
        wiringPiSPIDataRW(1, ch0, 3);
	wiringPiSPIDataRW(1, buffer[0][1], 3);
	//ch0 = (int)buffer;
	wiringPiSPIDataRW(1, ch1, 3);
	wiringPiSPIDataRW(1, buffer[1][1], 3);
	//ch1 = (int)buffer;
	wiringPiSPIDataRW(1, ch2, 3);
	wiringPiSPIDataRW(1, buffer[2][1], 3);
	//ch2 = (int)buffer;
        std:: cout << buffer[0][1] << buffer[1][1] << buffer[2][1];
    pthread_exit(NULL);
}*/

// read a channel
void mcp3008_read(uint8_t adcnum)
{
	unsigned int commandout = 0;
	unsigned int adcout = 0;

	commandout = adcnum & 0x3;  // only 0-7
	commandout |= 0x18;     // start bit + single-ended bit

	uint8_t spibuf[3];

	spibuf[0] = commandout;
	spibuf[1] = 0;
	spibuf[2] = 0;

	wiringPiSPIDataRW(1, spibuf, 3);

	adcout = ((spibuf[1] << 8) | (spibuf[2])) >> 4;

	printf("%d\n", adcout);

}


int main(){

	wiringPiSetupGpio();
	wiringPiSPISetup(1, 4*1000*1000);
	delay(50);
	for (;;){
		mcp3008_read(1);
		delay(50);
	}

	return 0;
}



