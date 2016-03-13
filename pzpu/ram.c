/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdlib.h>
#include <string.h>
#include "ram.h"
#include "pfmt.h"

//#define EMBED_AVR 1

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

/************************** CACHING FUNCTIONS **************************/

#if RAM_ICACHE
static uint32_t icache_base = 0;
static uint8_t icache_data[RAM_ICACHE];
static uint8_t icache_vlen = 0;

static void ram_icache_init()
{
	icache_base = 0;
	icache_vlen = ram_rd_seq(0,RAM_ICACHE,icache_data);
	if (icache_vlen == RAM_ICACHE) {
#ifdef RAM_DBG
		msg(0,"ICache of "PFMT_8UINT" bytes successfully initialized.\n",icache_vlen);
#endif
	}
}

/*
inline static void ram_icache_sync()
{
	if (icache_vlen)
		ram_wr_seq(icache_base,icache_vlen,icache_data);
}
*/

inline static uint8_t ram_icache(uint32_t adr)
{
	if ((adr < icache_base) || (adr >= icache_base + icache_vlen)) {
		//ram_icache_sync();
		icache_base = adr;
		icache_vlen = ram_rd_seq(adr,RAM_ICACHE,icache_data);

		if (!icache_vlen) return 0;
		return icache_data[0];
	}
	return icache_data[adr-icache_base];
}
#endif /* ICACHE */

#if RAM_SCACHE
#define RAM_SGAP (((RAM_SCACHE - 1) / 2) * 4)
static uint32_t scache_base = 0;
static uint32_t scache_data[RAM_SCACHE];
static uint16_t scache_vlen = 0;

inline static void ram_scache_sync()
{
	if (scache_vlen) //there's something to write
		ram_wr_seq(scache_base,scache_vlen,(uint8_t*)scache_data);
}

inline static uint8_t ram_scache_check(uint32_t adr)
{
	if ((adr < scache_base) || (adr >= scache_base + scache_vlen)) {
		ram_scache_sync();
		if (adr < RAM_SGAP) scache_base = 0; //wtf? we're at the memory start point
		else scache_base = (adr & (~3)) - RAM_SGAP;
		scache_vlen = ram_rd_seq(scache_base,RAM_SCACHE*4,(uint8_t*)scache_data);

		if (scache_vlen & 3) {
			scache_vlen = scache_vlen & (~3);
			if (!scache_vlen) {
#ifdef RAM_DBG
				msg(1,"SCache: Vlen = "PFMT_16UINT"\n",scache_vlen);
#endif
#ifdef RAM_OUT_ABORT
				abort();
#endif
				return 0;
			}
		}
	}
	return 1;
}

inline static uint32_t ram_scache_r(uint32_t adr)
{
	if (!ram_scache_check(adr)) return 0;
	return scache_data[(adr - scache_base)/4];
}

inline static void ram_scache_w(uint32_t adr, uint32_t val)
{
	if (!ram_scache_check(adr)) return;
	scache_data[(adr - scache_base)/4] = val;
}

static void ram_scache_init(uint32_t sp)
{
	//just read without sync
	scache_vlen = 0;
	ram_scache_r(sp);
}
#endif /* SCACHE */

/************************** HOST-SPECIFIC **************************/

#if EMBED_AVR /* HOST = 8-bit AVR MCU */

int ram_init(uint32_t sz)
{
	uint32_t smpl;

	//sample first and last locations
	if (!sd_raw_read(img_offset,(uint8_t*)&smpl,4))
		return 1;

	if (!sd_raw_read(img_offset+sz-1,(uint8_t*)&smpl,4))
		return 2;

	ramsize = sz;

#ifdef RAM_DBG
	msg(0,"RAM of "PFMT_32UINT" bytes successfully initialized.\n",sz);
#endif

#if RAM_ICACHE
	ram_icache_init();
#endif
#if RAM_SCACHE
	ram_scache_init(sz-4);
#endif

	return 0;
}

void ram_release()
{
#ifdef RAM_DBG
	msg(0,"Releasing RAM... ");
#endif

#if RAM_ICACHE
//	ram_icache_sync();
#endif
#if RAM_SCACHE
	ram_scache_sync();
#endif

	sd_raw_sync();

#ifdef RAM_DBG
	msg(0,"done.\n");
#endif
}

uint8_t ram_rd_b(uint32_t adr)
{
#if RAM_ICACHE
	return ram_icache(adr);
#else
	uint8_t r = 0;
	if (adr >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [RD B] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
		return 0;
	}
	if (sd_raw_read(img_offset+adr,&r,1))
		return r;

#ifdef RAM_DBG
	msg(1,"Unable to read [B] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
	return 0;
#endif /* ICACHE */
}

/*
void ram_wr_b(uint32_t adr, uint8_t val)
{
	if (adr >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR B] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
		return;
	}
	sd_raw_read(img_offset+adr,&val,1);
}
*/

uint32_t ram_rd_dw(uint32_t adr)
{
#if RAM_SCACHE
	return swap(ram_scache_r(adr));
#else
	uint32_t r = 0;
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [RD DW] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
		return 0;
	}
	if (sd_raw_read(img_offset+adr,(uint8_t*)&r,4))
		return swap(r);

#ifdef RAM_DBG
	msg(1,"Unable to read [DW] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
	return 0;
#endif /* SCACHE */
}

void ram_wr_dw(uint32_t adr, uint32_t val)
{
#if RAM_SCACHE
	ram_scache_w(adr,swap(val));
#else
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR DW] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
		return;
	}
	val = swap(val);
	sd_raw_write(img_offset+adr,(uint8_t*)&val,4);
#endif /* SCACHE */
}

uint8_t ram_rd_seq(uint32_t start, uint8_t len, uint8_t* buf)
{
	if (start >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [RD SQ] (0x"PFMT_32XINT")\n",start);
#endif
		return 0;
	}
	if (start + len >= ramsize)
		len = ramsize - start;

	if (sd_raw_read(img_offset+start,buf,len)) return len;
	else {
#ifdef RAM_DBG
		msg(1,"Unable to read "PFMT_8UINT" bytes from 0x"PFMT_32XINT"\n",len,start);
#endif
		return 0;
	}
}

uint8_t ram_wr_seq(uint32_t start, uint8_t len, const uint8_t* buf)
{
	if (start >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR SQ] (0x"PFMT_32XINT")\n",start);
#endif
		return 0;
	}
	if (start + len >= ramsize)
		len = ramsize - start;

	if (sd_raw_write(img_offset+start,buf,len)) return len;
	else {
#ifdef RAM_DBG
		msg(1,"Unable to write "PFMT_8UINT" bytes to 0x"PFMT_32XINT"\n",len,start);
#endif
		return 0;
	}
}

#else /* HOST = PC (x86) */

#include <byteswap.h> //for endianness conversion

int ram_init(uint32_t sz)
{
	if (RAM) ram_release();
	RAM = (uint8_t*)malloc(sz);
	if (!RAM) return 1;
	ramsize = sz;
	memset(RAM,0,sz);

#ifdef RAM_DBG
	msg(0,"RAM of "PFMT_32UINT" bytes successfully initialized.\n",sz);
#endif

#if RAM_ICACHE
	ram_icache_init();
#endif
#if RAM_SCACHE
	ram_scache_init(sz-4);
#endif

	return 0;
}

void ram_release()
{
#if RAM_ICACHE
	//ram_icache_sync();
#endif
#if RAM_SCACHE
	ram_scache_sync();
#endif

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
		msg(1,"File too large (%lu bytes), RAM size is only "PFMT_32UINT" bytes.\n",sz,maxsz);
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

	//Do cache initializers again
#if RAM_ICACHE
	ram_icache_init();
#endif
#if RAM_SCACHE
	ram_scache_init(sz-4);
#endif

	return 0;
}
#endif /* RAM_OS_ENABLED */

void ram_wr_dw(uint32_t adr, uint32_t val)
{
#if RAM_SCACHE
	ram_scache_w(adr,__bswap_32(val));
#else
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR DW] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return;
	}
	//big endian:
	/*adr += 3;
	RAM[adr--] = val & 0xff;
	RAM[adr--] = (val >>= 8) & 0xff;
	RAM[adr--] = (val >>= 8) & 0xff;
	RAM[adr]   = (val >>  8);*/
	val = __bswap_32(val);
	memcpy(RAM+adr,&val,4);
#endif /* SCACHE */
}

uint32_t ram_rd_dw(uint32_t adr)
{
#if RAM_SCACHE
	return __bswap_32(ram_scache_r(adr));
#else
	uint32_t r = 0;
	if (adr >= ramsize-3) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [RD DW] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return r;
	}
	//big endian:
	/*r  = RAM[adr++]; r <<= 8;
	r |= RAM[adr++]; r <<= 8;
	r |= RAM[adr++]; r <<= 8;
	r |= RAM[adr];
	return r;*/
	memcpy(&r,RAM+adr,4);
	return __bswap_32(r);
#endif /* SCACHE */
}

/*
void ram_wr_b(uint32_t adr, uint8_t val)
{
	if (adr >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR B] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return;
	}
	RAM[adr] = val;
}
*/

uint8_t ram_rd_b(uint32_t adr)
{
#if RAM_ICACHE
	return ram_icache(adr);
#else
	if (adr >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [RD B] (adr = 0x"PFMT_32XINT")\n",adr);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return 0;
	}
	return RAM[adr];
#endif /* ICACHE */
}

uint8_t ram_rd_seq(uint32_t start, uint8_t len, uint8_t* buf)
{
	if (start >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [RD SQ] (0x"PFMT_32XINT")\n",start);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return 0;
	}
	if (start + len >= ramsize)
		len = ramsize - start;

	memcpy(buf,RAM+start,len);
	return len;
}

uint8_t ram_wr_seq(uint32_t start, uint8_t len, const uint8_t* buf)
{
	if (start >= ramsize) {
#ifdef RAM_DBG
		msg(1,"Reached out of memory [WR SQ] (0x"PFMT_32XINT")\n",start);
#endif
#ifdef RAM_OUT_ABORT
		abort();
#endif
		return 0;
	}
	if (start + len >= ramsize)
		len = ramsize - start;

	memcpy(RAM+start,buf,len);
	return len;
}

#endif /* Main implementation switch */
