/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "io.h"
#include "pzpu.h"
#include "pfmt.h"

#ifdef PZPU_IOINTERACT
#include <stdio.h>
#endif


void term_putchar(const uint32_t x)
{
#if PZPU_IOINTERACT
	putchar(x & 0xFF);
#endif
}

uint32_t term_getchar(void)
{
#if PZPU_IOINTERACT
	return (0x100 | getchar()); //Receive is valid
#else
	return 0; //Receive is invalid
#endif
}
