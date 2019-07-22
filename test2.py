#!/usr/bin/python3

"""
Names: Vacquero
Student Number: AGSVAC001
Prac: AGSVAC001
Date: 21/07/2019
"""

try:
    import RPi.GPIO as GPIO
except RuntimeError:
    print("Use sudo")

#Pin mode
GPIO.setmode(GPIO.BOARD)
chan_in = 29
chan_out = 36
GPIO.setup(chan_out, GPIO.OUT)
GPIO.setup(chan_in, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)


def main():
    print("working")
    GPIO.add_event_detect(channel, GPIO.RISING)
    GPIO.add_event_callback(channel, my_callback_one,  bouncetime=200) 

def my_callback_one():
    GPIO.output(36, 1)
    return
    
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
<<<<<<< HEAD
    except e:
        GPIO.cleanup()
        print("Some other error occurred")
        print(e.message)

    
=======
    
>>>>>>> daeb7d39dd4f3f3babcefae610358346b7846783