/*
 * Prac4.cpp
 * 
 * Originall written by Stefan Schröder and Dillion Heald
 * 
 * Adapted for EEE3096S 2019 by Keegan Crankshaw

 */

#include "Prac4.h"

using namespace std;

bool playing = true; // should be set false when paused
bool stopped = false; // If set to true, program should close
unsigned char buffer[2][BUFFER_SIZE][2];
int buffer_location = 0;
bool bufferReading = 0; //using this to switch between column 0 and 1 - the first column
bool threadReady = false; //using this to finish writing the first column at the start of the song, before the column is played
long lastInterruptTime = 0;

// Configure your interrupts here.
// Don't forget to use debouncing.
void play_pause_isr(void){
        printf("Pause");
	long interruptTime = millis();
	if (interruptTime-lastInterruptTime>200){
		playing = !playing;
        }
		lastInterruptTime = interruptTime;
    //Write your logis here
}

void stop_isr(void){
    // Write your logic here
	long interruptTime = millis();
	if (interruptTime-lastInterruptTime>200){
		stopped = !stopped;
                exit(0);
          }
		lastInterruptTime = interruptTime;
}

/*
 * Setup Function. Called once 
 */
int setup_gpio(void){
    //Set up wiring Pi
    wiringPiSetup();
    //setting up the buttons
    pinMode(2,INPUT);
    pullUpDnControl(2,PUD_DOWN);
    wiringPiISR(2,INT_EDGE_RISING, &play_pause_isr);
    pinMode(3,INPUT);
    pullUpDnControl(3,PUD_DOWN);
    wiringPiISR(3,INT_EDGE_RISING, &stop_isr);


    //setting up the SPI interface
    wiringPiSPISetup(0,500000);
    return 0;
}

/* 
 * Thread that handles writing to SPI
 * 
 * You must pause writing to SPI if not playing is true (the player is paused)
 * When calling the function to write to SPI, take note of the last argument.
 * You don't need to use the returned value from the wiring pi SPI function
 * You need to use the buffer_location variable to check when you need to switch buffers
 */
void *playThread(void *threadargs){
    // If the thread isn't ready, don't do anything
    while(!threadReady){
        continue;}
    
    //You need to only be playing if the stopped flag is false
    while(!stopped){
        //Code to suspend playing if paused
		//Interrupts used for this
        
        //Write the buffer out to SPI
        if (playing==true){
        printf("Also here");
        wiringPiSPIDataRW(0, buffer[bufferReading][buffer_location], 2);

        }
		
        //Do some maths to check if you need to toggle buffers
        buffer_location++;
        if(buffer_location >= BUFFER_SIZE) {
            buffer_location = 0;
            bufferReading = !bufferReading; // switches column one it finishes one column
        }
    }
    
    pthread_exit(NULL);
}

int main(){
    // Call the setup GPIO function
	if(setup_gpio()==-1){
        return 0;
    }
    
    /* Initialize thread with parameters
     * Set the play thread to have a 99 priority
     * Read https://docs.oracle.com/cd/E19455-01/806-5257/attrib-16/index.html
     */ 
    
    //Write your logic here
    pthread_attr_t tattr;
    pthread_t thread_id;
    int newprio = 99;
    sched_param param;
    
    pthread_attr_init (&tattr);
    pthread_attr_getschedparam (&tattr, &param); /* safe to get existing scheduling param */
    param.sched_priority = newprio; /* set the priority; others are unchanged */
    pthread_attr_setschedparam (&tattr, &param); /* setting the new scheduling param */
    pthread_create(&thread_id, &tattr, playThread, (void *)1); /* with new priority specified *
    
    /*
     * Read from the file, character by character
     * You need to perform two operations for each character read from the file
     * You will require bit shifting
     * 
     * buffer[bufferWriting][counter][0] needs to be set with the control bits
     * as well as the first few bits of audio
     * 
     * buffer[bufferWriting][counter][1] needs to be set with the last audio bits
     * 
     * Don't forget to check if you have pause set or not when writing to the buffer
     * 
     */
     
    // Open the file
    char ch;
    FILE *filePointer;
    printf("%s\n", FILENAME);
    filePointer = fopen(FILENAME, "r"); // read mode

    if (filePointer == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int counter = 0;
    int bufferWriting = 0;

    // Have a loop to read from the file
	while((ch = fgetc(filePointer)) != EOF){
        while(threadReady && bufferWriting==bufferReading && counter==0){
            //waits in here after it has written to a side, and the thread is still reading from the other side
            continue;
        }
        //Set config bits for first 8 bit packet and OR with upper bits
        buffer[bufferWriting][counter][0] = 0b01110000;
        buffer[bufferWriting][counter][0] |= (ch>>6);
        printf("%x:", ch);
        //Set next 8 bit packet
        buffer[bufferWriting][counter][1] = (ch<<2);

        counter++;
        if(counter >= BUFFER_SIZE+1){
            if(!threadReady){
                threadReady = true;
            }

            counter = 0;
            bufferWriting = (bufferWriting+1)%2;
        }

    }
     
    // Close the file
    fclose(filePointer);
    printf("Complete reading"); 
	 
    //Join and exit the playthread
    pthread_join(thread_id, NULL); 
    pthread_exit(NULL);
	
    return 0;
}
