/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "pzpu.h"
#include "ram.h"
#include "io.h"
#include "pfmt.h"

static uint8_t idim,dtpc,halt = 1;
static uint32_t sp,pc,ram,cycle[2];

#ifdef PZPU_DBG

#include "debug.h"

#if PZPU_DBG >= 3

static unsigned long long ino = 0; //internal instruction number (for extended trace generation only)

void trace(uint8_t code, uint8_t ilong, uint8_t arg)
{
	if (code > 0x0f) {
		if (!ilong) return;
		msg(0,"%llu\tPC: 0x%08X  SP: 0x%08X [0x%08X]  I: 0x%02X S[" MNEMOUTP "] arg = %hhu\n",
				ino++,pc,sp,ram_rd_dw(sp),code,mnemonics[ilong],arg);
	} else {
		msg(0,"%llu\tPC: 0x%08X  SP: 0x%08X [0x%08X]  I: 0x%02X S[" MNEMOUTP "]\n",
				ino++,pc,sp,ram_rd_dw(sp),code,mnemonics[code]);
	}
}

#else /* PZPU_DBG lvl */

void trace(uint8_t code, uint8_t ilong, uint8_t arg)
{
	if ((code > 0x0f) && (!ilong)) return;
	msg(0,"PC:0x"PFMT_32XINT" SP:0x"PFMT_32XINT" [0x"PFMT_32XINT"] I:0x%02X [%hu]\n",pc,sp,ram_rd_dw(sp),code,arg);
}

#endif /* PZPU_DBG lvl */
#endif /* PZPU_DBG */

void reset(uint32_t ramsize)
{
	halt = 0;
	idim = 0;
	dtpc = 0;
	pc = 0x0;
	sp = ramsize - 8;
	ram = ramsize;
	reset_cycles();
}

inline static uint32_t mem_rd_dw(uint32_t adr)
{
	if (adr >= PZPU_IOSPACE)
		return io_rd(adr);
	else
		return ram_rd_dw(adr);
}

inline static void mem_wr_dw(uint32_t adr, uint32_t val)
{
	if (adr >= PZPU_IOSPACE)
		io_wr(adr,val);
	else
		ram_wr_dw(adr,val);
}

inline static zpuint pop(void)
{
	zpuint r = mem_rd_dw(sp);
	sp += 4;
	return r;
}

inline static void push(zpuint w)
{
	sp -= 4;
	mem_wr_dw(sp,w);
}

inline static zpuint flip(zpuint x)
{
	int i;
	zpuint y = 0;
	for (i = 0; i < 32; i++) {
		y |= x & 0x80000000;
		if (i < 31) { //don't shift last bit
			y >>= 1;
			x <<= 1;
		}
	}
	return y;
}

inline static void store(void)
{
	zpuint adr = pop();
	zpuint val = pop();
	mem_wr_dw(adr,val);
}

inline static void im(uint8_t x)
{
	zpuint v;
	if (idim) {
		//next IM
		v = (pop() << 7);
		push(v | x);
	} else {
		//first IM
		v = 0;
		uint8_t b = (x >> 6);
		if (b) {
			//propagate sign bit
			v = 0xffffff80;
		}
		push(v | x);
	}
#if PZPU_DBG >= 4
	msg(0,"IM: IDIM = %i X = 0x%02X  Result = 0x%08X\n",idim,x,(v|x));
#endif
	idim = 1;
}

inline static uint32_t sp_off(uint8_t w)
{
	uint32_t r = w;
	r *= 4;
	r += sp;
	return r;
}

inline static void iemu(uint8_t x)
{
	push(pc+1);
	pc = (uint32_t)x * 32;
	dtpc = 1;
}

static void exec(uint8_t x)
{
#if PZPU_DBG >= 2
	trace(x,0,0);
#endif

	switch (x) {
	case ZPU_BREAK:
		halt = 1;
		break;

	case ZPU_NOP:
		break;

	case ZPU_PUSHSP:
		push(sp);
		break;

	case ZPU_POPSP:
		sp = pop();
		break;

	case ZPU_POPPC:
		pc = pop();
		dtpc = 1;
		break;

	case ZPU_ADD:
		push(pop() + pop());
		break;

	case ZPU_OR:
		push(pop() | pop());
		break;

	case ZPU_AND:
		push(pop() & pop());
		break;

	case ZPU_NOT:
		push(~pop());
		break;

	case ZPU_FLIP:
		push(flip(pop()));
		break;

	case ZPU_LOAD:
		push(mem_rd_dw(pop()));
		break;

	case ZPU_STORE:
		store();
		break;

	default:

		if (x >= ZPU_IM) {
#if PZPU_DBG >= 2
			trace(x,16,x & 0x7f);
#endif
			im(x & 0x7f);
			return; //don't reset IDIM flag

		} else if (x >= ZPU_LOADSP) {
#if PZPU_DBG >= 2
			trace(x,18,(x & 0x1f) ^ 0x10);
#endif
			push(mem_rd_dw(sp_off((x & 0x1f) ^ 0x10)));

		} else if (x >= ZPU_STORESP) {
#if PZPU_DBG >= 2
			trace(x,17,(x & 0x1f) ^ 0x10);
#endif
			zpuint a = sp_off((x & 0x1f) ^ 0x10);
			mem_wr_dw(a,pop());

		} else if (x >= ZPU_EMULATE) {
#if PZPU_DBG >= 2
			trace(x,20,x & 0x1f);
#endif
			iemu(x & 0x1f);

		} else if (x >= ZPU_ADDSP) {
#if PZPU_DBG >= 2
			trace(x,19,x & 0x0f);
#endif
			zpuint a = sp_off(x & 0x0f);
			push((zpuint)mem_rd_dw(a) + pop());

		} else {
#ifdef PZPU_DBG
			msg(1,"Unknown instruction 0x%02X at PC = 0x%08X\n",x,pc);
#endif
			halt = 1;

		}
		break;
	}

	idim = 0;
}

void step(void)
{
	//Check HALT state
	if (halt) return;

	//Execute next instruction
	exec(mem_rd_b(pc));

	//Increment PC (if needed)
	if (!dtpc) pc++;
	dtpc = 0;

	//Increment 64-bit cycles counter
	if (!(++cycle[0])) cycle[1]++;
}

uint8_t status(void)
{
	return halt;
}

uint32_t get_cycles(uint8_t high)
{
	if (high) return cycle[1];
	return cycle[0];
}

void reset_cycles(void)
{
	cycle[0] = 0;
	cycle[1] = 0;
}
