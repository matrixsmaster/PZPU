/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdio.h>
#include "io.h"

void io_wr(uint32_t adr, uint32_t be)
{
#ifdef PZPU_IODBG
	if (be > 32 && be < 255)
		printf("IO WR to 0x%08X : %u [%c]\n",adr,be,(char)be);
	else
		printf("IO WR to 0x%08X : %u\n",adr,be);
#endif

	if (adr < EBOARD_OFFSET) {
		fprintf(stderr,"Unknown IO addr 0x%08X\n",adr);
		return;
	}
	adr -= EBOARD_OFFSET;

	switch (adr) {
	case BZPU_UARTTx:
		putchar(be & 0xFF);
		break;

	default:
		break;
	}
}

uint32_t io_rd(uint32_t adr)
{
#ifdef PZPU_IODBG
	printf("IO RD attempt : 0x%08X\n",adr);
#endif

	if (adr < EBOARD_OFFSET) {
		fprintf(stderr,"Unknown IO addr 0x%08X\n",adr);
		return 0;
	}
	adr -= EBOARD_OFFSET;

	switch (adr) {
	case BZPU_UARTTx:
		return 0x100; //Transmit buffer ready flag
		break;

	case BZPU_UARTRx:
#ifdef PZPU_IOINTERACT
		return (0x100 | getchar()); //Receive is valid
#else
		return 0; //Receive is invalid
#endif

	default:
		break;
	}
	return 0;
}
