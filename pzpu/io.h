/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef IO_H_
#define IO_H_

#include <inttypes.h>

#define EBOARD_OFFSET 0x080A0000U

void io_wr(uint32_t adr, uint32_t be);
uint32_t io_rd(uint32_t adr);

#endif /* IO_H_ */
