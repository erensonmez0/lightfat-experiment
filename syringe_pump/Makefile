# Makefile for Syringe Pump Simulator

CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm

TARGET = syringe_pump_sim
SRC = syringe_pump_sim.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
