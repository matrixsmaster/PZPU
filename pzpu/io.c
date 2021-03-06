/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "io.h"
#include "pzpu.h"
#include "pfmt.h"

#ifdef PZPU_IODBG
#include "debug.h"
#endif

static uint32_t sampled_cycles[2];


void io_wr(uint32_t adr, uint32_t be)
{
#ifdef PZPU_IODBG
	if (be > 32 && be < 255)
		msg(0,"IO WR to 0x"PFMT_32XINT" : "PFMT_32UINT" [%c]\n",adr,be,(char)be);
	else
		msg(0,"IO WR to 0x"PFMT_32XINT" : "PFMT_32UINT"\n",adr,be);
#endif

	if (adr < EBOARD_OFFSET) {
#ifdef PZPU_IODBG
		msg(1,"Unknown IO addr 0x"PFMT_32XINT"\n",adr);
#endif
		return;
	}
	adr -= EBOARD_OFFSET;

	switch (adr) {

	case BZPU_UARTTx:
		term_putchar(be);
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
	msg(0,"IO RD attempt : 0x"PFMT_32XINT"\n",adr);
#endif

	if (adr < EBOARD_OFFSET) {
#ifdef PZPU_IODBG
		msg(1,"Unknown IO addr 0x"PFMT_32XINT"\n",adr);
#endif
		return 0;
	}
	adr -= EBOARD_OFFSET;

	switch (adr) {

	case BZPU_UARTTx:
		return 0x100; //Transmit buffer ready flag
		break;

	case BZPU_UARTRx:
		return term_getchar();

	case BZPU_CntH:
		return sampled_cycles[1];

	case BZPU_CntL:
		return sampled_cycles[0];

	case BZPU_MHZ:
		return 1; //FIXME: do measurements and return real value

	default:
		break;
	}
	return 0;
}
