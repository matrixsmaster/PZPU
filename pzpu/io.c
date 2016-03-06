#include <stdio.h>
#include "io.h"

void io_wr(uint32_t adr, uint32_t be)
{
	if (be > 32 && be < 255)
		printf("IO WR to 0x%08X : %u [%c]\n",adr,be,(char)be);
	else
		printf("IO WR to 0x%08X : %u\n",adr,be);
}

uint32_t io_rd(uint32_t adr)
{
	printf("IO RD attempt : 0x%08X\n",adr);
	return 0;
}
