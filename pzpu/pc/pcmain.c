/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include "pzpu.h"
#include "ram.h"
#include "pfmt.h"

#ifndef MYSIZE
#define MYSIZE 0x00200000
#endif

#define BREAKA 0

#if MYSIZE == 0
extern uint32_t ramsize; //reference
#endif

int main(int argc, char* argv[])
{
	int r;
#if BREAKA > 0
	unsigned n = 0;
#endif

	//Check program arguments
	if (argc < 2) {
		printf("Usage: %s <.bin>\n",argv[0]);
		return 1;
	}

	//Try to init RAM
	r = ram_init(MYSIZE);
	if (r) {
		printf("Unable to allocate RAM (code %d)\n",r);
		return 2;
	}

	//And load it with binary program
	r = ram_load(argv[1],0);
	if (r) {
		printf("Unable to load file (code %d)\n",r);
		ram_release();
		return 3;
	}

#if MYSIZE > 0
	reset(MYSIZE);
#else
	reset(ramsize);
#endif

	while (!status()) {
		step();
#if BREAKA > 0
		if (++n >= BREAKA) {
			printf(""PFMT_32UINT" cycles passed, press Enter to continue...\n",n);
			getchar();
			n = 0;
		}
#endif
	}

	ram_release();

	printf("Simulation cycles counter = 0x"PFMT_32XINT PFMT_32XINT"\n",get_cycles(1),get_cycles(0));

	return 0;
}
