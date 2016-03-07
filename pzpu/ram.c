/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ram.h"

static uint8_t* RAM = NULL;
static uint32_t ramsize = 0;

void ram_init(uint32_t sz)
{
	if (RAM) ram_release();
	RAM = (uint8_t*)malloc(sz);
	ramsize = sz;
	memset(RAM,0,sz);

#ifdef RAM_DBG
	printf("RAM of %u bytes successfully initialized.\n",sz);
#endif
}

void ram_release()
{
	if (!RAM) return;
	free(RAM);
	ramsize = 0;
	RAM = NULL;

#ifdef RAM_DBG
	printf("RAM released.\n");
#endif
}

int ram_load(const char* fn, uint32_t maxsz)
{
	size_t sz;
	FILE* f = fopen(fn,"r");
	if (!f) return -1;

	fseek(f,0,SEEK_END);
	sz = ftell(f);
	if (sz >= maxsz) {
#ifdef RAM_DBG
		fprintf(stderr,"File too large (%lu bytes), RAM size is only %u bytes.\n",sz,maxsz);
#endif
		fclose(f);
		return 1;
	}
	fseek(f,0,SEEK_SET);

	if (fread(RAM,sz,1,f) < 1) {
#ifdef RAM_DBG
		fprintf(stderr,"Can't load all the bytes.\n");
#endif
		fclose(f);
		return 2;
	}

#ifdef RAM_DBG
	printf("File '%s' data (%lu bytes) successfully loaded into RAM.\n",fn,sz);
#endif

	fclose(f);
	return 0;
}

void ram_wr_dw(uint32_t adr, uint32_t val)
{
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		fflush(stdout);
		fprintf(stdout,"Reached out of memory [WR DW] (adr = 0x%08X)\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return;
	}
	adr += 3;
	RAM[adr--] = val & 0xff;
	RAM[adr--] = (val >>= 8) & 0xff;
	RAM[adr--] = (val >>= 8) & 0xff;
	RAM[adr]   = (val >>  8);
}

uint32_t ram_rd_dw(uint32_t adr)
{
	uint32_t r = 0;
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		fflush(stdout);
		fprintf(stdout,"Reached out of memory [RD DW] (adr = 0x%08X)\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return r;
	}
	r  = RAM[adr++]; r <<= 8;
	r |= RAM[adr++]; r <<= 8;
	r |= RAM[adr++]; r <<= 8;
	r |= RAM[adr];
	return r;
}

void ram_wr_b(uint32_t adr, uint8_t val)
{
	if (adr >= ramsize) {
#ifdef RAM_DBG
		fprintf(stderr,"Reached out of memory [WR B] (adr = 0x%08X)\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return;
	}
	RAM[adr] = val;
}

uint8_t ram_rd_b(uint32_t adr)
{
	if (adr >= ramsize) {
#ifdef RAM_DBG
		fprintf(stderr,"Reached out of memory [RD B] (adr = 0x%08X)\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return 0;
	}
	return RAM[adr];
}
