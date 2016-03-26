# PZPU - Pseudo-ZPU emulator
# (C) MatrixS_Master, 2016
# GPL v2
#

.PHONY: all
all:
	make -f Makefile.pc

.PHONY: atmega attiny
atmega:
	make -f Makefile.atmega
	
attiny:
	make -f Makefile.attiny
	
.PHONY: clean
clean:
	make -f Makefile.pc clean
	make -f Makefile.atmega clean
	make -f Makefile.attiny clean
