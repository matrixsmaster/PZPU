
#ifndef RAM_H_
#define RAM_H_

#include <inttypes.h>

void ram_init(uint32_t sz);
void ram_release();

int ram_load(const char* fn, uint32_t maxsz);

void mem_wr_dw(uint32_t adr, uint32_t val);
void mem_wr_b(uint32_t adr, uint8_t val);

uint32_t mem_rd_dw(uint32_t adr);
uint8_t mem_rd_b(uint32_t adr);

#endif /* RAM_H_ */
