/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <simpletools.h>
#include "io.h"
#include "pzpu.h"
#include "pfmt.h"

void term_putchar(const uint32_t x)
{
	putchar(x & 0xFF);
}

uint32_t term_getchar(void)
{
	return (0x100 | getchar()); //Receive is valid
}
