/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef RAM_AVR_H_
#define RAM_AVR_H_

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "ram.h"
#include "pfmt.h"
#include "avr_io.h"

#if EMBED_AVR_MEGA
#include "mega/avr_config.h"
#include "mega/sd_raw.h"

#elif EMBED_AVR_TINY
#include "tiny/avr_config.h"
#include "tiny/sd_raw.h"

#else
#error "Unknown AVR port"
#endif

#ifdef RAM_DBG
#include "debug.h"
#endif

uint32_t swap(uint32_t x);

extern uint32_t img_offset;
extern uint32_t ramsize; //TODO: merge it with img_length!

#endif /* RAM_AVR_H_ */
