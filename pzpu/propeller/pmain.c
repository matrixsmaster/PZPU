/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <simpletools.h>
//#include <stdio.h>
//#include <propeller.h>
#include "prop_config.h"
#include "debug.h"
#include "io.h"
#include "ram.h"
#include "pzpu.h"
#include "pfmt.h"

//extern _Driver _SimpleSerialDriver;
//extern _Driver _FileDriver;
//
//_Driver *_driverlist[] = {
//&_SimpleSerialDriver,
//&_FileDriver,
//NULL
//};

#ifndef MYSIZE
//#define MYSIZE 0x00200000
#define MYSIZE 0
#endif
//
////FIXME: move them to config header
//#define DO 3
//#define CLK 2
//#define DI 1
//#define CS 0

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

    //waitcnt(CLKFREQ+CNT);
	printf("Mounting...\n");

    //Mount SD card
    r = sd_mount(PIN_DO,PIN_SCK,PIN_DI,PIN_SS);
    printf("SD mount: %d\n",r);

	//Try to init RAM
	r = ram_init(MYSIZE);
	if (r) {
		printf("Unable to allocate RAM (code %d)\n",r);
		return 0;
	}

	//And load it with binary program
	r = ram_load("testzpu.bin",0);
	if (r) {
		printf("Unable to load file (code %d)\n",r);
		ram_release();
		return 0;
	}

#if !PROP_DRYRUN
#if MYSIZE > 0
	reset(MYSIZE);
#else
	reset(ramsize);
#endif
#endif /* !PROP_DRYRUN */

    printf("Running...\n");
    _DIRA |= (1<<16);
	while (!status()) {
		step();
#if BREAKA > 0
		if (++n >= BREAKA) {
			printf(""PFMT_32UINT" cycles passed, press Enter to continue...\n",n);
			getchar();
			n = 0;
		}
#endif
        _OUTA ^= (1<<16);
	}

	ram_release();

	printf("Simulation cycles counter = 0x"PFMT_32XINT PFMT_32XINT"\n",get_cycles(1),get_cycles(0));

	return 0;
}
