/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef RAM_H_
#define RAM_H_

#include <inttypes.h>

//Debug output enable
#define RAM_DBG 1

//Enable OS functions (file I/O)
//#define RAM_OS_ENABLED 1

//Abort on boundary tresspassing
//#define RAM_OUT_ABORT 1

//Prototypes

int ram_init(uint32_t sz);
void ram_release();

int ram_load(const char* fn, uint32_t maxsz);

void ram_wr_dw(uint32_t adr, uint32_t val);
void ram_wr_b(uint32_t adr, uint8_t val);

uint32_t ram_rd_dw(uint32_t adr);
uint8_t ram_rd_b(uint32_t adr);

#endif /* RAM_H_ */
