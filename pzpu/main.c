/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include "pzpu.h"
#include "ram.h"

#define MYSIZE 0x00200000
#define BREAKA 0

int main(int argc, char* argv[])
{
#if BREAKA > 0
	unsigned n = 0;
#endif

	if (argc < 2) {
		printf("Usage: %s <.bin>\n",argv[0]);
		return 1;
	}

	if (ram_init(MYSIZE)) {
		printf("Unable to allocate RAM (try lesser size value)\n");
		return 2;
	}
	if (ram_load(argv[1],MYSIZE)) {
		printf("Unable to load file!\n");
		ram_release();
		return 3;
	}

	reset(MYSIZE);

	while (!status()) {
		step();
#if BREAKA > 0
		if (++n >= BREAKA) {
			printf("%u cycles passed, press Enter to continue...\n",n);
			getchar();
			n = 0;
		}
#endif
	}

	ram_release();

	printf("Simulation cycles counter = 0x%08X%08X\n",get_cycles(1),get_cycles(0));

	return 0;
}
