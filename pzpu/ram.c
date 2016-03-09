/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdlib.h>
#include <string.h>
#include "ram.h"

#define EMBED_AVR 1

#if RAM_OS_ENABLED
#include <stdio.h>
#elif EMBED_AVR
#include "avr/avr_io.h"
#include "avr/sd_raw.h"
#endif

#ifdef RAM_DBG
#include "debug.h"
#endif

#if !EMBED_AVR
static uint8_t* RAM = NULL;
#endif

static uint32_t ramsize = 0;

#if EMBED_AVR

int ram_init(uint32_t sz)
{
	uint32_t smpl = 0xABCDEF01;

	//sample first and last locations
	if (!sd_raw_write(img_offset,(uint8_t*)&smpl,4))
		return 1;

	if (!sd_raw_write(img_offset+sz-1,(uint8_t*)&smpl,4))
		return 2;

	if (!sd_raw_read(img_offset,(uint8_t*)&smpl,4))
		return 3;
	if (smpl != 0xABCDEF01)
		return 4;

	if (!sd_raw_read(img_offset+sz-1,(uint8_t*)&smpl,4))
		return 5;
	if (smpl != 0xABCDEF01)
		return 6;

	ramsize = sz;

#ifdef RAM_DBG
	msg(0,"RAM of %u bytes successfully initialized.\n",sz);
#endif

	return 0;
}

void ram_release()
{
#ifdef RAM_DBG
	msg(0,"Releasing RAM... ");
#endif

	sd_raw_sync();

#ifdef RAM_DBG
	msg(0,"done.\n");
#endif
}

uint8_t ram_rd_b(uint32_t adr)
{
	uint8_t r = 0;
	if (adr >= ramsize) return 0;
	if (sd_raw_read(img_offset+adr,&r,1))
		return r;
	else
		return 0;
}

void ram_wr_b(uint32_t adr, uint8_t val)
{
	if (adr >= ramsize) return;
	sd_raw_read(img_offset+adr,&val,1);
}

uint32_t ram_rd_dw(uint32_t adr)
{
	uint32_t r = 0;
	if (adr >= ramsize-3) return 0;
//	r  = ram_rd_b(adr++); r <<= 8;
//	r |= ram_rd_b(adr++); r <<= 8;
//	r |= ram_rd_b(adr++); r <<= 8;
//	r |= ram_rd_b(adr);
//	return r;
	if (sd_raw_read(img_offset+adr,(uint8_t*)&r,4))
		return r;
	else
		return 0;
}

void ram_wr_dw(uint32_t adr, uint32_t val)
{
	if (adr >= ramsize-3) return;
//	adr += 3;
//	ram_wr_b(adr--,val);
//	ram_wr_b(adr--,val >>= 8);
//	ram_wr_b(adr--,val >>= 8);
//	ram_wr_b(adr,val >>  8);
	sd_raw_write(img_offset+adr,(uint8_t*)&val,4);
}

#else

int ram_init(uint32_t sz)
{
	if (RAM) ram_release();
	RAM = (uint8_t*)malloc(sz);
	if (!RAM) return 1;
	ramsize = sz;
	memset(RAM,0,sz);

#ifdef RAM_DBG
	msg(0,"RAM of %u bytes successfully initialized.\n",sz);
#endif

	return 0;
}

void ram_release()
{
	if (!RAM) return;
	free(RAM);
	ramsize = 0;
	RAM = NULL;

#ifdef RAM_DBG
	msg(0,"RAM released.\n");
#endif
}

#ifdef RAM_OS_ENABLED
int ram_load(const char* fn, uint32_t maxsz)
{
	size_t sz;
	FILE* f = fopen(fn,"r");
	if (!f) return -1;

	fseek(f,0,SEEK_END);
	sz = ftell(f);
	if (sz >= maxsz) {
#ifdef RAM_DBG
		msg(1,"File too large (%lu bytes), RAM size is only %u bytes.\n",sz,maxsz);
#endif
		fclose(f);
		return 1;
	}
	fseek(f,0,SEEK_SET);

	if (fread(RAM,sz,1,f) < 1) {
#ifdef RAM_DBG
		msg(1,"Can't load all the bytes.\n");
#endif
		fclose(f);
		return 2;
	}

#ifdef RAM_DBG
	msg(0,"File '%s' data (%lu bytes) successfully loaded into RAM.\n",fn,sz);
#endif

	fclose(f);
	return 0;
}
#endif /* RAM_OS_ENABLED */

void ram_wr_dw(uint32_t adr, uint32_t val)
{
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR DW] (adr = 0x%08X)\n",adr);
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
		msg(1,"Reached out of memory [RD DW] (adr = 0x%08X)\n",adr);
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
		msg(1,"Reached out of memory [WR B] (adr = 0x%08X)\n",adr);
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
		msg(1,"Reached out of memory [RD B] (adr = 0x%08X)\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return 0;
	}
	return RAM[adr];
}

#endif /* Main implementation switch */
