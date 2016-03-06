#include <stdio.h>
#include <stdlib.h>
#include "pzpu.h"
#include "ram.h"

#define MYSIZE 65536

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Usage: %s <.bin>\n",argv[0]);
		return 1;
	}

	ram_init(MYSIZE);
	if (ram_load(argv[1],MYSIZE)) {
		printf("Unable to read file!\n");
		ram_release();
		return 2;
	}

	reset(MYSIZE);

	while (!status()) step();

	ram_release();

	return 0;
}
