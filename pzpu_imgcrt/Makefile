# PZPU - Pseudo-ZPU emulator
# (C) MatrixS_Master, 2016
# GPL v2
#

CC = gcc
CCFLAGS = -O2 -g -Wall
LDFLAGS = -lm
APP = pzpu-imgcrt

OBJS = main.o

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(LDFLAGS) -o $(APP) $(OBJS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(APP)
