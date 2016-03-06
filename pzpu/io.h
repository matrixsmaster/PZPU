
#ifndef IO_H_
#define IO_H_

#include <inttypes.h>

void io_wr(uint32_t adr, uint32_t be);
uint32_t io_rd(uint32_t adr);

#endif /* IO_H_ */
