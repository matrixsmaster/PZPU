#if 0
void putchar(int a);
int printf(char * a);
int puts(char * a);
char getchar();
#else
#include <stdio.h>
#include "testzpu.h"
//#include "f86-combined.h"
#endif

#define int long long int
#define RAMSIZE (1024*1024*32)

int RAM[RAMSIZE];
int PC = 0;
int SP = RAMSIZE - 8;
int PChalt = 0;
int cycle = 0;
int halt = 1;
int idim = 0;
int i_hold = 0;
int i_addr = -1;

int bor()
{
}

int band(int a, int b)
{
    return a & b;
}

int bxor()
{
}

int blshift()
{
}

int brshift()
{
}

int get_dword(int addr)
{
	if (addr < 0 || addr >= RAMSIZE) {
		int ret = 0;
		addr = addr - 0x080A0000;
		if (addr < 0) {
			printf("RD violation @ 0x%08X: address below zero",addr);
		} else if (addr == 0xC) {
			ret = 0x100; //TX ready flag
		} else if (addr == 0x10) {
            printf("Waiting for user input...\n");
			halt = 1; //stop right there to not waste too much CPU time
		} else {
			printf("RD violation @ 0x%08X: IO space access violation",addr);
		}
		return ret;
	}

	if (!band(addr,3)) {
		return RAM[addr >> 2];
	} else {
        printf("RD violation @ 0x%08X: reminder is %u",addr,band(addr,3));
		return 0;
	}
}

int get_word(int addr)
{
	int r = get_dword(band(addr,0xFFFFFFFC));
	if (!band(addr,2)) {
		return r >> 16;
	} else {
		return band(r,0xFFFF);
	}
}

int get_byte(int addr)
{
	int s = 8 * (3 - band(addr,3));
	return band((get_dword(band(addr,0xFFFFFFFC)) >> s),0xFF);
}

void wr_dword(int addr, int w)
{
	if (addr < 0 || addr >= RAMSIZE) {
		if (addr < 0) {
			printf("WR violation @ 0x%08X: address below zero",addr);
		} else if (addr == 0x080A000C) {
			putchar(w);
		} else {
//			violation(addr,0,4,"IO space access violation")
		}
	} else if (!band(addr,3)) {
		RAM[addr >> 2] = band(w,0xFFFFFFFF);
	} else {
		printf("WR violation @ 0x%08X: reminder is %u",addr,band(addr,3));
	}
}

void wr_word(int addr, int w)
{
	int r = get_dword(band(addr,0xFFFFFFFC));
	w = band(w,0xFFFF);
	if (!band(addr,2)) {
		wr_dword(band(addr,0xFFFFFFFC),(w << 16) | band(r,0xFFFF));
	} else {
		wr_dword(band(addr,0xFFFFFFFC),band(r,0xFFFF0000) | w);
	}
}

void wr_byte(int addr, int d)
{
	int r = get_dword(band(addr,0xFFFFFFFC));
	int s = 8 * (3 - band(addr,3));
	int m = ~(0xFF << s);
	r = band(r,m) | (band(d,0xFF) << s);
	wr_dword(band(addr,0xFFFFFFFC),r);
}

int pop()
{
	int r = get_dword(SP);
	SP = SP + 4;
	return r;
}

void push(int w)
{
	SP = SP - 4;
	wr_dword(SP,w);
}

int flip(int x)
{
	int y = 0;
	for (int i = 0; i < 33; i++) {
		y = y | band(x,0x80000000);
		if (i < 31) { //don't shift last bit
			y = y >> 1;
			x = x << 1;
		}
	}
	return y;
}

void im(int x)
{
	int v;
	if (idim != 0) {
		//next IM
		v = (pop() << 7);
		push(v | x);
	} else {
		//first IM
		v = 0;
		int b = (x >> 6);
		if (b != 0) {
			//propagate sign bit
			v = 0xffffff80;
		}
		push(v | x);
	}
	idim = 1;
}

int sp_off(int w)
{
	return band(((w << 2) + SP),0xFFFFFFFF);
}

void iemu(char x)
{
	push(PC+1);
	PC = band((x * 32),0xFFFFFFFF);
	PChalt = 1;
}

void exec(int x)
{
    //printf("Executing %d\n",x);
    
	if (x == 0) {
		halt = 1;

	} else if (x == 2) { //ZPU_PUSHSP
		push(SP);

	} else if (x == 4) { //ZPU_POPPC
		PC = pop();
		PChalt = 1;

	} else if (x == 5) { //ZPU_ADD
		push(pop() + pop());

	} else if (x == 7) { //ZPU_OR
		push(pop() | pop());

	} else if (x == 6) { //ZPU_AND
		push(band(pop(),pop()));

	} else if (x == 8) { //ZPU_LOAD
		push(get_dword(pop()));

	} else if (x == 9) { //ZPU_NOT
		push(~pop());

	} else if (x == 10) { //ZPU_FLIP
		push(flip(pop()));

	} else if (x == 12) { //ZPU_STORE
		int adr = pop();
		int val = pop();
		wr_dword(adr,val);

	} else if (x == 13) { //ZPU_POPSP
		SP = pop();

	} else if (x == 11) { //ZPU_NOP

	} else if (band(x,0x80)) { //ZPU_IM
		im(band(x,0x7f));

	} else if (x >= 0x60) { //ZPU_LOADSP
		push(get_dword(sp_off(band(x,0x1f) ^ 0x10)));

	} else if (x >= 0x40) { //ZPU_STORESP
		int a = sp_off(band(x,0x1f) ^ 0x10);
		wr_dword(a,pop());

	} else if (x >= 0x20) { //ZPU_EMULATE
		iemu(band(x,0x1f));

	} else if (x >= 0x10) { //ZPU_ADDSP
		int a = sp_off(band(x,0x0f));
		push(band((get_dword(a) + pop()),0xFFFFFFFF));

	} else {
		printf("Unknown instruction %hhu\n",x);
		halt = 1;
	}

	if (x < 0x80) {
		idim = 0;
	}
}

void step()
{
	if (halt) return;

    int n_addr = band(PC,0xFFFFFFFC);
	if (n_addr != i_addr) {
		i_addr = n_addr;
		i_hold = get_dword(i_addr);
		if (band(PC,0x03) > 0) {
			i_hold = band((i_hold << (8 * band(PC,3))),0xFFFFFFFF);
		}
	}

	//printf("ihold = 0x%08X\n",i_hold);
	exec(i_hold >> 24);

	if (!PChalt) {
		PC = PC + 1;
		i_hold = band((i_hold << 8),0xFFFFFFFF);
	} else {
		i_addr = -1;
	}
	PChalt = 0;
	cycle = cycle + 1;
}

void reset()
{
	// prepare CPU registers
	PC = 0;
	SP = RAMSIZE - 8;
	PChalt = 0;
	idim = 0;
	i_addr = -1;
	i_hold = 0;

	// prepare global CPU state
	cycle = 0;
	halt = 0;
}

int main(int argc, char* argv[])
{
    int n = 0;
    for (int i = 0; i < blob_len/4; i++) {
        int x = 0;
        for (int j = 0; j < 4; j++) {
            x = (x << 8) | blob[n++];
        }
        RAM[i] = x;
    }
    reset();
    while (!halt) {
        step();
        //getchar();
    }
    puts("Quit.");
    return 0;
}
