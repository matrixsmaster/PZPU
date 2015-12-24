# Makefile :)

CC=avr-gcc
AZ=avr-size
OC=avr-objcopy

MCU=atmega1284p
APP=avr8086
OBJS=8086.o

CCFLAGS=-O2 -mmcu=$(MCU) -Wall -fsigned-char

all: $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(APP).elf
	$(AZ) -Ax $(OBJS)
	$(OC) -j .text -j .data -O ihex $(APP).elf $(APP).hex

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o
