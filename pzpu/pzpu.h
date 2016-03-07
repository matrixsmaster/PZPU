/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef PZPU_H_
#define PZPU_H_

#include <inttypes.h>

//PZPU debug level (undef to disable debug output)
#define PZPU_DBG 2

//Transparent byte memory IO
#define mem_rd_b ram_rd_b
#define mem_wr_b ram_wr_b

//Main type of emulated 32-bit value
typedef uint32_t zpuint;

//General IO space start point
#define PZPU_IOSPACE 0x08000000U

enum ZpuOpcode {
	ZPU_BREAK = 0,
	ZPU_PUSHSP = 2,
	ZPU_POPPC = 4,
	ZPU_ADD = 5,
	ZPU_OR = 7,
	ZPU_AND = 6,
	ZPU_LOAD = 8,
	ZPU_NOT = 9,
	ZPU_FLIP = 10,
	ZPU_STORE = 12,
	ZPU_POPSP = 13,
	ZPU_NOP = 11,
	ZPU_IM = 0x80,
	ZPU_STORESP = 0x40,
	ZPU_LOADSP = 0x60,
	ZPU_ADDSP = 0x10,
	ZPU_EMULATE = 0x20
};

#ifdef PZPU_DBG
#define DEFMNEMLEN 10
#define MNEMOUTP "%10s"

static const char mnemonics[0x10+5][DEFMNEMLEN] = {
		"BREAK",
		"N/A",
		"PUSHSP",
		"N/A",
		"POPPC",
		"ADD",
		"AND",
		"OR",
		"LOAD",
		"NOT",
		"FLIP",
		"NOP",
		"STORE",
		"POPSP",
		"N/A",
		"N/A",
		"IM",
		"STORESP",
		"LOADSP",
		"ADDSP",
		"EMULATE"
};
#endif /* PZPU_DBG */

//Emulator function prototypes
void reset(uint32_t ramsize);
void step();
uint8_t status();


#endif /* PZPU_H_ */
