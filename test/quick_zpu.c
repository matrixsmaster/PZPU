/* 
 * Quick ZPU
 * Part of PZPU - Pseudo-ZPU emulator
 * 
 * (C) Dmitry 'MatrixS_Master' Solovyev, 2019
 * License: GPL v2
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <inttypes.h>

#define RAMsize (1024 * 1024 * 32)

uint32_t* RAM = NULL;
uint32_t PC = 0;
uint32_t SP = RAMsize - 8;
uint32_t PChalt = false;
uint32_t cycle = 0;
bool halt = true;
uint32_t idim = 0;
uint32_t i_hold = 0;
uint32_t i_addr = -1;

uint32_t get_dword(uint32_t addr)
{
	if (addr < 0 || addr >= RAMsize) {
		uint32_t ret = 0;
		addr = addr - 0x080A0000;
		if (addr < 0) {
			printf("RD violation @ 0x%08X: address below zero",addr);
		} else if (addr == 0xC) {
			ret = 0x100; //TX ready flag
		} else if (addr == 0x10) {
            printf("Waiting for user input...\n");
			halt = true; //stop right there to not waste too much CPU time
		} else {
			printf("RD violation @ 0x%08X: IO space access violation",addr);
		}
		return ret;
	}

	if (!(addr & 3)) {
		return RAM[addr >> 2];
	} else {
        printf("RD violation @ 0x%08X: reminder is %u",addr,addr & 3);
		return 0;
	}
}

uint32_t get_word(uint32_t addr)
{
	uint32_t r = get_dword(addr & 0xFFFFFFFC);
	if (!(addr & 2)) {
		return r >> 16;
	} else {
		return r & 0xFFFF;
	}
}

uint32_t get_byte(uint32_t addr)
{
	uint32_t s = 8 * (3 - (addr & 3));
	return (get_dword(addr & 0xFFFFFFFC) >> s) & 0xFF;
}

void wr_dword(uint32_t addr, uint32_t w)
{
	if (addr < 0 || addr >= RAMsize) {
		if (addr < 0) {
			printf("WR violation @ 0x%08X: address below zero",addr);
		} else if (addr == 0x080A000C) {
			putchar(w);
            printf("\nCycle = %u\n",cycle);
            abort();
		} else {
//			violation(addr,false,4,"IO space access violation")
		}
	} else if (!(addr & 3)) {
		RAM[addr >> 2] = w & 0xFFFFFFFF;
	} else {
		printf("WR violation @ 0x%08X: reminder is %u",addr,addr & 3);
	}
}

void wr_word(uint32_t addr, uint32_t w)
{
	uint32_t r = get_dword(addr & 0xFFFFFFFC);
	w = w & 0xFFFF;
	if (!(addr & 2)) {
		wr_dword(addr & 0xFFFFFFFC,(w << 16) | (r & 0xFFFF));
	} else {
		wr_dword(addr & 0xFFFFFFFC,(r & 0xFFFF0000) | w);
	}
}

void wr_byte(uint32_t addr, uint32_t d)
{
	uint32_t r = get_dword(addr & 0xFFFFFFFC);
	uint32_t s = 8 * (3 - (addr & 3));
	uint32_t m = ~(0xFF << s);
	r = (r & m) | ((d & 0xFF) << s);
	wr_dword(addr & 0xFFFFFFFC,r);
}

uint32_t pop()
{
	uint32_t r = get_dword(SP);
	SP = SP + 4;
	return r;
}

void push(uint32_t w)
{
	SP = SP - 4;
	wr_dword(SP,w);
}

uint32_t flip(uint32_t x)
{
	uint32_t y = 0;
	for (uint32_t i = 0; i < 33; i++) {
		y = y | (x & 0x80000000);
		if (i < 31) { //don't shift last bit
			y = y >> 1;
			x = x << 1;
		}
	}
	return y;
}

void im(uint32_t x)
{
	uint32_t v;
	if (idim != 0) {
		//next IM
		v = (pop() << 7);
		push(v | x);
	} else {
		//first IM
		v = 0;
		uint32_t b = (x >> 6);
		if (b != 0) {
			//propagate sign bit
			v = 0xffffff80;
		}
		push(v | x);
	}
	idim = 1;
}

uint32_t sp_off(uint32_t w)
{
	return ((w << 2) + SP) & 0xFFFFFFFF;
}

bool exec_ext(uint8_t x)
{
	if ((x == 2)) { //loadh
		push(get_word(pop()));

	} else if (x == 3) { //storeh
		uint32_t adr = pop();
		uint32_t val = pop();
		wr_word(adr,val);

	} else if (x == 4) { //lessthan
		int32_t a = (int32_t)(pop());
		int32_t b = (int32_t)(pop());
		push(a < b? 1:0);

	} else if (x == 5) { //lessthanorequal
		int32_t a = (int32_t)(pop());
		int32_t b = (int32_t)(pop());
		push(a <= b? 1:0);

	} else if (x == 6) { //ulessthan
		uint32_t a = pop();
		uint32_t b = pop();
		push(a < b? 1:0);

	} else if (x == 7) { //ulessthanorequal
		uint32_t a = pop();
		uint32_t b = pop();
		push(a <= b? 1:0);

//	} else if (x == 8) { //swap

	} else if (x == 9) { //mult
		push(pop() * pop());

	} else if (x == 10) { //lshiftright
		uint32_t shift = pop() & 0xFFFFFFFF;
		uint32_t val = pop() & 0xFFFFFFFF;
		push(val >> (shift & 0x3f));

	} else if (x == 11) { //ashiftleft
		uint32_t shift = pop() & 0xFFFFFFFF;
		uint32_t val = pop() & 0xFFFFFFFF;
		push(val << (shift & 0x3f));

	} else if (x == 12) { //ashiftright
		uint32_t shift = pop() & 0xFFFFFFFF;
		int32_t val = (int32_t)(pop());
		push(val >> (shift & 0x3f));

	} else if (x == 13) { //call
		uint32_t adr = pop();
		push(PC+1);
		PC = adr;
		PChalt = true;

	} else if (x == 14) { //eq
		push(pop() == pop() ? 1:0);

	} else if (x == 15) { //neq
        push(pop() != pop() ? 1:0);

	} else if (x == 16) { //neg
		push(-((int32_t)(pop())));

	} else if (x == 17) { //sub
		uint32_t a = pop();
		uint32_t b = pop();
		push(b-a);

	} else if (x == 18) { //xor
		push(pop() ^ pop());

	} else if (x == 19) { //loadb
		push(get_byte(pop()));

	} else if (x == 20) { //storeb
		uint32_t adr = pop();
		uint32_t val = pop() & 0xFF;
		wr_byte(adr,val);

//	} else if (x == 21) { //div
//	} else if (x == 22) { //mod

	} else if (x == 23) { //eqbranch
		uint32_t target = pop() + PC;
		if (pop() == 0) {
			PC = target;
			PChalt = true;
		}

	} else if (x == 24) { //neqbranch
		uint32_t target = pop() + PC;
		if (pop() != 0) {
			PC = target;
			PChalt = true;
		}

	} else if (x == 25) { //poppcrel
		PC = pop() + PC;
		PChalt = true;

//	} else if (x == 26) { //config
//	} else if (x == 28) { //syscall

	} else if (x == 29) { //pushspadd
		uint32_t a = SP;
		uint32_t w = pop() * 4;
		push(w + a);

//	} else if (x == 30) { //mult16x16

	} else if (x == 31) { //callpcrel
		uint32_t adr = pop();
		push(PC+1);
		PC = PC + adr;
		PChalt = true;

	} else {
		printf("Extended inst. %hhu was scheduled for normal execution, but not yet implemented\n",x);
		return false;
	}
	return true;
}

void iemu(uint8_t x)
{
#if 0
	if (exec_ext(x)) {
		return;
	}
#endif
	push(PC+1);
	PC = (x * 32) & 0xFFFFFFFF;
	PChalt = true;
}

void exec(uint8_t x)
{
    //printf("Executing %hhu\n",x);
    
	if (x == 0) {
		halt = true;

	} else if (x == 2) { //ZPU_PUSHSP
		push(SP);

	} else if (x == 4) { //ZPU_POPPC
		PC = pop();
		PChalt = true;

	} else if (x == 5) { //ZPU_ADD
		push(pop() + pop());

	} else if (x == 7) { //ZPU_OR
		push(pop() | pop());

	} else if (x == 6) { //ZPU_AND
		push(pop() & pop());

	} else if (x == 8) { //ZPU_LOAD
		push(get_dword(pop()));

	} else if (x == 9) { //ZPU_NOT
		push(~pop());

	} else if (x == 10) { //ZPU_FLIP
		push(flip(pop()));

	} else if (x == 12) { //ZPU_STORE
		uint32_t adr = pop();
		uint32_t val = pop();
		wr_dword(adr,val);

	} else if (x == 13) { //ZPU_POPSP
		SP = pop();

	} else if (x == 11) { //ZPU_NOP

	} else if (x >= 0x80) { //ZPU_IM
		im(x & 0x7f);

	} else if (x >= 0x60) { //ZPU_LOADSP
		push(get_dword(sp_off((x & 0x1f) ^ 0x10)));

	} else if (x >= 0x40) { //ZPU_STORESP
		uint32_t a = sp_off((x & 0x1f) ^ 0x10);
		wr_dword(a,pop());

	} else if (x >= 0x20) { //ZPU_EMULATE
		iemu(x & 0x1f);

	} else if (x >= 0x10) { //ZPU_ADDSP
		uint32_t a = sp_off(x & 0x0f);
		push((get_dword(a) + pop()) & 0xFFFFFFFF);

	} else {
		printf("Unknown instruction %hhu\n",x);
		halt = true;
	}

	if (x < 0x80) {
		idim = 0;
	}
}

void step()
{
	if (halt) return;

	if ((PC & 0xFFFFFFFC) != i_addr) {
		i_addr = PC & 0xFFFFFFFC;
		i_hold = get_dword(i_addr);
		if ((PC & 0x03) > 0) {
			i_hold = (i_hold << (8 * (PC & 3))) & 0xFFFFFFFF;
		}
	}

//	print(string.format("ihold = 0x%X",i_hold))
	exec(i_hold >> 24);

	if (!PChalt) {
		PC = PC + 1;
		i_hold = (i_hold << 8) & 0xFFFFFFFF;
	} else {
		i_addr = -1;
	}
	PChalt = false;
	cycle = cycle + 1;
}

void reset()
{
	// prepare CPU registers
	PC = 0;
	SP = RAMsize - 8;
	PChalt = false;
	idim = 0;
	i_addr = -1;
	i_hold = 0;

	// prepare global CPU state
	cycle = 0;
	halt = false;
}

int main(int argc, char* argv[])
{
    assert(argc > 1);
    
    FILE* f = fopen(argv[1],"rb");
    assert(f);

    RAM = (uint32_t*)malloc(RAMsize);

	bool op = true;
    uint8_t ptr[4];
	for (uint32_t i = 0; i < (RAMsize >> 2); i++) {
		if (op) {
			if (fread(ptr,4,1,f)) {
				uint32_t q = 0;
				for (uint32_t j = 0; j < 4; j++) {
					if (ptr[j]) {
						q = q | (ptr[j] << (8 * (3-j)));
					}
					//printf("saving 0x%08X\n",q);
				}
				RAM[i] = q;
			} else {
				RAM[i] = 0;
				op = false;
			}
		} else {
			RAM[i] = 0;
		}
	}
    fclose(f);
    
    reset();
    while (!halt) step();
    
    free(RAM);
    return 0;
}
