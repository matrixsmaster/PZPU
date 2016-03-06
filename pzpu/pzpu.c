#include "pzpu.h"
#include "ram.h"
#include "io.h"

static uint8_t idim,dtpc,halt = 1;
static uint32_t sp,pc,ram;

#ifdef PZPU_DBG

#include <stdio.h>
#include <stdarg.h>

void msg(const char* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);
	vprintf(fmt,vl);
	va_end(vl);
}

void trace(uint8_t code, uint8_t ilong, uint8_t arg)
{
	if (code > 0x0f) {
		if (!ilong) return;
		msg("PC: 0x%08X  SP: 0x%08X [0x%08X]  I: 0x%02X S[" MNEMOUTP "] arg = %hhu\n",pc,sp,ram_rd_dw(sp),code,mnemonics[ilong],arg);
	} else {
		msg("PC: 0x%08X  SP: 0x%08X [0x%08X]  I: 0x%02X S[" MNEMOUTP "]\n",pc,sp,ram_rd_dw(sp),code,mnemonics[code]);
	}
}

#endif /* PZPU_DBG */

void reset(uint32_t ramsize)
{
	halt = 0;
	idim = 0;
	dtpc = 0;
	pc = 0x0;
	sp = ramsize - 1;// - PZPU_STACKSZ;
	ram = ramsize;
}

static uint32_t inline mem_rd_dw(uint32_t adr)
{
	if (adr >= PZPU_IOSPACE)
		return io_rd(adr);
	else
		return ram_rd_dw(adr);
}

static void inline mem_wr_dw(uint32_t adr, uint32_t val)
{
	if (adr >= PZPU_IOSPACE)
		io_wr(adr,val);
	else
		ram_wr_dw(adr,val);
}

static zpuint inline pop(void)
{
	zpuint r = mem_rd_dw(sp);
	sp += 4;
	return r;
}

static void inline push(zpuint w)
{
	sp -= 4;
	mem_wr_dw(sp,w);
}

static zpuint inline flip(zpuint x)
{
	int i;
	zpuint y = 0;
	for (i = 0; i < 32; i++) {
		y |= x & 0x80000000;
		y >>= 1;
		x <<= 1;
	}
	return y;
}

static void inline store(void)
{
	zpuint adr = pop();
	zpuint val = pop();
	mem_wr_dw(adr,val);
}

static void inline im(uint8_t x)
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
			v = 0xffffff10;
		}
		push(v | x);
	}
#ifdef PZPU_DBG
	msg("IM: IDIM = %i X = 0x%02X  Result = 0x%08X\n",idim,x,(v|x));
#endif
	idim = 1;
}

static uint32_t inline sp_off(uint8_t w)
{
	uint32_t r = w;
	r *= 4;
	r += sp;
	return r;
}

static void inline iemu(uint8_t x)
{
	push(pc);
	pc = (uint32_t)x * 32;
	dtpc = 1;
}

static void exec(uint8_t x)
{
#ifdef PZPU_DBG
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
#ifdef PZPU_DBG
			trace(x,16,x & 0x7f);
#endif
			im(x & 0x7f);
			return; //don't reset IDIM flag

		} else if (x >= ZPU_LOADSP) {
#ifdef PZPU_DBG
			trace(x,18,(x & 0x1f) ^ 0x10);
#endif
			push(mem_rd_dw(sp_off((x & 0x1f) ^ 0x10)));

		} else if (x >= ZPU_STORESP) {
#ifdef PZPU_DBG
			trace(x,17,(x & 0x1f) ^ 0x10);
#endif
			zpuint a = sp_off((x & 0x1f) ^ 0x10);
			mem_wr_dw(a,pop());

		} else if (x >= ZPU_EMULATE) {
#ifdef PZPU_DBG
			trace(x,20,x & 0x1f);
#endif
			iemu(x & 0x1f);

		} else if (x >= ZPU_ADDSP) {
#ifdef PZPU_DBG
			trace(x,19,x & 0x0f);
#endif
			zpuint a = sp_off(x & 0x0f);
			push((zpuint)mem_rd_dw(a) + pop());

		} else {
#ifdef PZPU_DBG
			msg("Unknown instruction 0x%02X at PC = 0x%08X\n",x,pc);
#endif
			halt = 1;

		}
		break;
	}

	idim = 0;
}

void step(void)
{
	if (halt) return;
	exec(mem_rd_b(pc));
	if (!dtpc) pc++;
	dtpc = 0;
}

uint8_t status()
{
	return halt;
}
