/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef RAM_H_
#define RAM_H_

#include <inttypes.h>

//Debug output enable
//#define RAM_DBG 1

//Enable OS functions (file I/O)
//#define RAM_OS_ENABLED 1

//Abort on boundary tresspassing
#define RAM_OUT_ABORT 1

//Use instruction cache
#define RAM_ICACHE 32

//Use stack cache
#define RAM_SCACHE 15

//Prototypes

int ram_init(uint32_t sz);
void ram_release();

int ram_load(const char* fn, uint32_t maxsz);

void ram_wr_dw(uint32_t adr, uint32_t val);
void ram_wr_b(uint32_t adr, uint8_t val);

uint32_t ram_rd_dw(uint32_t adr);
uint8_t ram_rd_b(uint32_t adr);

uint8_t ram_rd_seq(uint32_t start, uint8_t len, uint8_t* buf);
uint8_t ram_wr_seq(uint32_t start, uint8_t len, const uint8_t* buf);

void ram_icache_init();
uint8_t ram_icache(uint32_t adr);

void ram_scache_sync();
uint8_t ram_scache_check(uint32_t adr);
uint32_t ram_scache_r(uint32_t adr);
void ram_scache_w(uint32_t adr, uint32_t val);
void ram_scache_init(uint32_t sp);


#endif /* RAM_H_ */
