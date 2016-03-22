/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdlib.h>
#include <string.h>
#include "ram.h"
#include "pfmt.h"

#ifdef RAM_DBG
#include "debug.h"
#endif

uint32_t ramsize = 0;

/************************** CACHING FUNCTIONS **************************/

#if RAM_ICACHE
static uint32_t icache_base = 0;
static uint8_t icache_data[RAM_ICACHE];
static uint8_t icache_vlen = 0;

void ram_icache_init()
{
	icache_base = 0;
	icache_vlen = ram_rd_seq(0,RAM_ICACHE,icache_data);
	if (icache_vlen == RAM_ICACHE) {
#ifdef RAM_DBG
		msg(0,"ICache of "PFMT_8UINT" bytes successfully initialized.\n",icache_vlen);
#endif
	}
}

inline uint8_t ram_icache(uint32_t adr)
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

inline void ram_scache_sync()
{
	if (scache_vlen) //there's something to write
		ram_wr_seq(scache_base,scache_vlen,(uint8_t*)scache_data);
}

inline uint8_t ram_scache_check(uint32_t adr)
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

inline uint32_t ram_scache_r(uint32_t adr)
{
	if (!ram_scache_check(adr)) return 0;
	return scache_data[(adr - scache_base)/4];
}

inline void ram_scache_w(uint32_t adr, uint32_t val)
{
	if (!ram_scache_check(adr)) return;
	scache_data[(adr - scache_base)/4] = val;
}

void ram_scache_init(uint32_t sp)
{
	//just read without sync
	scache_vlen = 0;
	ram_scache_r(sp);
#ifdef RAM_DBG
	msg(0,"SCache of "PFMT_16UINT" bytes successfully initialized.\n",RAM_SCACHE*4);
#endif
}
#endif /* SCACHE */

