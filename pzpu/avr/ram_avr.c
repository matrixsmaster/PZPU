/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "ram_avr.h"

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

