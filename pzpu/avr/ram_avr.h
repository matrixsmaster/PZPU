/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef RAM_AVR_H_
#define RAM_AVR_H_

#include <stdlib.h>
#include <string.h>
#include "ram.h"
#include "pfmt.h"
#include "avr_io.h"

#if EMBED_AVR_MEGA
#include "mega/sd_raw.h"
#elif EMBED_AVR_TINY
#include "tiny/sd_raw.h"
#else
#error "Unknown AVR port"
#endif

#ifdef RAM_DBG
#include "debug.h"
#endif

extern uint32_t ramsize;

#endif /* RAM_AVR_H_ */
