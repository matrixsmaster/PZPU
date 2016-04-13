/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ram.h"
#include "pfmt.h"
#include <byteswap.h> //for endianness conversion

#ifdef RAM_DBG
#include "debug.h"
#endif

#ifdef RAM_FILE

static FILE* RAM = NULL;
extern uint32_t ramsize;

int ram_init(uint32_t sz)
{
	//release file handle before create a new one
	if (RAM) ram_release();

	//open a memory file
	RAM = fopen(RAM_FILE,"r+");
	if (!RAM) return 1;

	//check the file size
	fseek(RAM,0,SEEK_END);
	size_t fsz = ftell(RAM);
	if ((!sz) && fsz) sz = fsz; //use RAM file size as RAM size
	else if (fsz < sz) {
		ram_release();
		return 2;
	}

	//set RAM size
	ramsize = sz;

#ifdef RAM_DBG
	msg(0,"RAM of "PFMT_32UINT" bytes successfully initialized.\n",sz);
#endif

	//Initialize caches
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
	fclose(RAM);
	ramsize = 0;
	RAM = NULL;

#ifdef RAM_DBG
	msg(0,"RAM released.\n");
#endif
}

int ram_load(const char* fn, uint32_t maxsz)
{
	size_t sz;
	uint8_t buf[RAM_BUFSIZE];
	int r;

	FILE* f = fopen(fn,"r");
	if (!f) return -1;
	if (maxsz == 0) maxsz = ramsize; //use whole memory if no constraint applied

	//Check the file size
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

	//Copy contents
	fseek(RAM,0,SEEK_SET);
	while (sz) {
		r = fread(buf,1,RAM_BUFSIZE,f);
		if (!r) {
#ifdef RAM_DBG
			msg(1,"Can't load all the bytes.\n");
#endif
			fclose(f);
			return 2;
		}
		fwrite(buf,r,1,RAM);
		sz -= r;
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
	val = __bswap_32(val);
	if (!fseek(RAM,adr,SEEK_SET)) {
		fwrite(&val,4,1,RAM);
	}
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
	if (fseek(RAM,adr,SEEK_SET)) return 0; //fail
	if (!fread(&r,4,1,RAM)) return 0; //fail
	return __bswap_32(r);
#endif /* SCACHE */
}

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

	uint8_t r;
	if (fseek(RAM,adr,SEEK_SET)) return 0; //fail
	if (!fread(&r,1,1,RAM)) return 0; //fail
	return r;
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

	if (fseek(RAM,start,SEEK_SET)) return 0; //fail
	return fread(buf,1,len,RAM);
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

	if (fseek(RAM,start,SEEK_SET)) return 0; //fail
	return fwrite(buf,1,len,RAM);
}

#endif /* RAM_FILE */
