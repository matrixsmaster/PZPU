/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef UTILS_PROP_H_
#define UTILS_PROP_H_


inline static uint32_t swap(uint32_t x)
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

#endif
