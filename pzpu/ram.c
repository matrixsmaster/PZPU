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
	printf("RAM of %u bytes successfully initialized.\n",sz);
}

void ram_release()
{
	if (!RAM) return;
	free(RAM);
	ramsize = 0;
	RAM = NULL;
	printf("RAM released.\n");
}

int ram_load(const char* fn, uint32_t maxsz)
{
	size_t sz;
	FILE* f = fopen(fn,"r");
	if (!f) return -1;

	fseek(f,0,SEEK_END);
	sz = ftell(f);
	if (sz >= maxsz) {
		fprintf(stderr,"File too large (%lu bytes), RAM size is only %u bytes.\n",sz,maxsz);
		fclose(f);
		return 1;
	}
//	sz--;
	fseek(f,0,SEEK_SET);

	if (fread(RAM,sz,1,f) < 1) {
		fprintf(stderr,"Can't load all the bytes.\n");
		fclose(f);
		return 2;
	}

	printf("File '%s' data (%lu bytes) successfully loaded into RAM.\n",fn,sz);
	fclose(f);
	return 0;
}

void ram_wr_dw(uint32_t adr, uint32_t val)
{
	if (adr >= ramsize-3) {
		fprintf(stderr,"Reached out of memory [WR DW] (adr = 0x%08X)\n",adr);
		return;
	}
	RAM[adr++] = val & 0xff;
	RAM[adr++] = (val >>= 8) & 0xff;
	RAM[adr++] = (val >>= 8) & 0xff;
	RAM[adr]   = (val >>  8);
}

uint32_t ram_rd_dw(uint32_t adr)
{
	uint32_t r = 0;
	if (adr >= ramsize-3) {
		fprintf(stderr,"Reached out of memory [RD DW] (adr = 0x%08X)\n",adr);
		return r;
	}
	adr += 3;
	r  = RAM[adr--]; r <<= 8;
	r |= RAM[adr--]; r <<= 8;
	r |= RAM[adr--]; r <<= 8;
	r |= RAM[adr];
	return r;
}

void ram_wr_b(uint32_t adr, uint8_t val)
{
	if (adr >= ramsize) {
		fprintf(stderr,"Reached out of memory [WR B] (adr = 0x%08X)\n",adr);
		return;
	}
	RAM[adr] = val;
}

uint8_t ram_rd_b(uint32_t adr)
{
	if (adr >= ramsize) {
		fprintf(stderr,"Reached out of memory [RD B] (adr = 0x%08X)\n",adr);
		return 0;
	}
	return RAM[adr];
}
