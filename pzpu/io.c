/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "io.h"
#include "pzpu.h"

static uint32_t sampled_cycles[2];

#ifdef PZPU_IOINTERACT
#include <stdio.h>
#endif

#ifdef PZPU_IODBG
#include "debug.h"
#endif

void io_wr(uint32_t adr, uint32_t be)
{
#ifdef PZPU_IODBG
	if (be > 32 && be < 255)
		msg(0,"IO WR to 0x%08X : %u [%c]\n",adr,be,(char)be);
	else
		msg(0,"IO WR to 0x%08X : %u\n",adr,be);
#endif

	if (adr < EBOARD_OFFSET) {
#ifdef PZPU_IODBG
		msg(1,"Unknown IO addr 0x%08X\n",adr);
#endif
		return;
	}
	adr -= EBOARD_OFFSET;

	switch (adr) {

	case BZPU_UARTTx:
#ifdef PZPU_IOINTERACT
		putchar(be & 0xFF);
#endif
		break;

	case BZPU_CntL:
		if (be & 1)
			reset_cycles();
		else if (be & 2) {
			sampled_cycles[0] = get_cycles(0);
			sampled_cycles[1] = get_cycles(1);
		}
		break;

	default:
		break;
	}
}

uint32_t io_rd(uint32_t adr)
{
#ifdef PZPU_IODBG
	msg(0,"IO RD attempt : 0x%08X\n",adr);
#endif

	if (adr < EBOARD_OFFSET) {
#ifdef PZPU_IODBG
		msg(1,"Unknown IO addr 0x%08X\n",adr);
#endif
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

	case BZPU_CntH:
		return sampled_cycles[1];

	case BZPU_CntL:
		return sampled_cycles[0];

	case BZPU_MHZ:
		return 1;

	default:
		break;
	}
	return 0;
}
