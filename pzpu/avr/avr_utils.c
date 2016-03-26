/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "ram_avr.h"

void term_putchar(const uint32_t x)
{
#if LCD_SIZEW && LCD_SIZEH
	LCDWriteChar(x & 0xFF);
#else
	USARTWriteChar(x & 0xFF);
#endif /* LCD */
}

uint32_t term_getchar(void)
{
	return (0x100 | USARTReadChar()); //Receive is valid
}

inline uint32_t swap(uint32_t x)
{
	uint32_t y = (x & 0xff);
	y <<= 8; x >>= 8;
	y |= (x & 0xff);
	y <<= 8; x >>= 8;
	y |= (x & 0xff);
	y <<= 8; x >>= 8;
	y |= x;
    return y;
}
