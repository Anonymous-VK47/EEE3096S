#!/usr/bin/python3
"""
Names: Vacquero
Student Number: AGSVAC001
Prac: AGSVAC001
Date: 21/07/2019
"""

#Fixed Counter
count = 0

# import Relevant Librares
import RPi.GPIO as GPIO

#Pin mode
GPIO.setmode(GPIO.BOARD)
#Channels, Setup and Pull up resistors
chan_in = [29,31]
GPIO.setup(chan_in, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
chan_out = [36,38,40]
GPIO.setup(chan_out, GPIO.OUT)

# Find out what value needs to be displayed
def find_value(x):
    bin_count = bin(x) # Convert decimal value to binary value
    # Convert binary value to string
    if x<2:
        string_bin = "00"+str(bin_count)[2:] #Append 00 to front if number less than 2
    elif x<4:
        string_bin = "0"+str(bin_count)[2:] #Append 0 to front if number less than 4
    else:
        string_bin = str(bin_count)[2:] #[2:] gets rid of the '0b' resulting from converting to binary

    #Set each GPIO with corresponding binary value
    #List indexed and strings converted to ints
    GPIO.output(chan_out, (int(string_bin[0:1]), int(string_bin[1:2]), int(string_bin[2:]))) 
    return
#Run when subtract button pressed
def my_cb1(channel):
    global count #Set count as global variable
    if count == 0: #Value at the extremes for wrap around
        count = 7
        find_value(count) #Use function defined above to find binary value
    else:
        count -= 1 #Decrement when button pressed
        find_value(count) #Use function defined above to find binary value
    return

#Run when add button pressed
def my_cb2(channel):
    global count #Set count as global variable
    if count == 7: #Value at the extremes for wrap around
        count = 0
        find_value(count) #Use function defined above to find binary value
    else:
        count += 1 #Increment when button pressed
        find_value(count) #Use function defined above to find binary value
    return

GPIO.add_event_detect(29, GPIO.RISING, callback=my_cb1, bouncetime=200) #Check if subtract button pressed
GPIO.add_event_detect(31, GPIO.RISING, callback=my_cb2, bouncetime=200) #Check if add button pressed


def main():
    global channel
    if GPIO.event_detected(29): #Enter if subtract button Pressed
        my_cb1 #Call Function
    if GPIO.event_detected(31): #Enter if add button Pressed
        my_cb2 #Call Function
    b = input("Press Ctrl+C to exit")

# Only run the functions if 
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    except e:
        GPIO.cleanup()
        print("Some other error occurred")
        print(e.message)
