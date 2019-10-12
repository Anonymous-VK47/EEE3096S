.RECIPEPREFIX +=
CC = g++
CFLAGS = -Wall -lm -lrt -lwiringPi -lpthread

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/conversion1.cpp -o obj/conversion1
    $(CC) $(CFLAGS) obj/conversion1 -o bin/conversion1

run:
    sudo ./bin/conversion1

clean:
    rm $(PROG) $(OBJS)
