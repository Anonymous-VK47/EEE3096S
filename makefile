.RECIPEPREFIX +=
CC = g++
CFLAGS = -Wall -lm -lrt -lwiringPi -lpthread

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/conversion.cpp -o obj/conversion
    $(CC) $(CFLAGS) obj/conversion -o bin/conversion

run:
    sudo ./bin/conversion

clean:
    rm $(PROG) $(OBJS)
