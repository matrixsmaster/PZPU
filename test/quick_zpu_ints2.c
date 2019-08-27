#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "testzpu.h"

#define int long long int

int RAM[2097152];
int PC = 0;
int SP = 2097152 - 8;
int PChalt = 0;
int cycle = 0;
int halt = 1;
int idim = 0;
int i_hold = 0;
int i_addr = -1;

int blshift(int v, int n)
{
    //int lv = v;
    //for (int i = 0; i < n; i++) {
    while (n--) {
        v += v;
        if (v > 4294967295) v -= 4294967296;
    }
    //printf("%08x << %08x = %08x\n",lv,n,v);
    //assert(v == (lv<<n));
    //assert(v < 4294967296);
    return v;
}

int brshift(int v, int n)
{
    //int lv = v;
    //for (int i = 0; i < n; i++) v /= 2;
    while (n--) v = v / 2;
    //assert(v == (lv>>n));
    //assert(v < 4294967296);
    return v;
}

int bor(int a, int b)
{
    //int la = a;
    //int lb = b;
    int c = 0;
    for (int x = 0; x < 32; x++) {
        c += c;
        if ((a >= 2147483648) || (b >= 2147483648)) c++;
        a += a;
        if (a > 4294967295) a -= 4294967296;
        b += b;
        if (b > 4294967295) b -= 4294967296;
    }
    //printf("%x | %x = %x\n",la,lb,c);
    //assert(c == (la|lb));
    //assert(c < 4294967296);
    return c;
}

int band(int a, int b)
{
    //int la = a;
    //int lb = b;
    int c = 0;
    while (a > 4294967295) a -= 4294967296;
    while (b > 4294967295) b -= 4294967296;
    for (int x = 0; x < 32; x++) {
        c += c;
        if ((a >= 2147483648) && (b >= 2147483648)) c++;
        a += a;
        if (a > 4294967295) a -= 4294967296;
        b += b;
        if (b > 4294967295) b -= 4294967296;
    }
    //printf("%lx & %lx = %lx\n",la,lb,c);
    //assert(c < 4294967296);
    //assert(c == (la&lb));
    return c;
}

int bxor(int a, int b)
{
    int la = a;
    int lb = b;
    int c = 0;
    for (int x = 0; x < 32; x++) {
        c += c;
        if (((a >= 2147483648) || (b >= 2147483648)) && ((a < 2147483648) || (b < 2147483648))) c++;
        a += a;
        if (a > 4294967295) a -= 4294967296;
        b += b;
        if (b > 4294967295) b -= 4294967296;
        printf("%lld\t%lld\t%lld\n",a,b,c);
    }
    printf("%lld ^ %lld = %lld\n",la,lb,c);
    assert(c < 4294967296);
    assert(c == (la^lb));
    return c;
}

int get_dword(int addr)
{
	if (addr < 0 || addr >= 2097152) {
		int ret = 0;
		addr = addr - 134873088;
		if (addr < 0) {
			printf("RD violation @ 0x%08X: address below zero",addr);
		} else if (addr == 12) {
			ret = 256; //TX ready flag
		} else if (addr == 16) {
            printf("Waiting for user input...\n");
			halt = 1; //stop right there to not waste too much CPU time
		} else {
			printf("RD violation @ 0x%08X: IO space access violation",addr);
		}
		return ret;
	}

	if (!band(addr,3)) {
        printf("RAM @ %d = %d\n",brshift(addr,2),RAM[brshift(addr,2)]);
		return RAM[brshift(addr,2)];
	} else {
        printf("RD violation @ 0x%08X: reminder is %u",addr,band(addr,3));
		return 0;
	}
}

int get_word(int addr)
{
	int r = get_dword(band(addr,4294967292));
	if (!band(addr,2)) {
		return brshift(r,16);
	} else {
		return band(r,65535);
	}
}

int get_byte(int addr)
{
	int s = 8 * (3 - band(addr,3));
	return band(brshift(get_dword(band(addr,4294967292)),s),255);
}

void wr_dword(int addr, int w)
{
	if (addr < 0 || addr >= 2097152) {
		if (addr < 0) {
			printf("WR violation @ 0x%08X: address below zero",addr);
		} else if (addr == 134873100) {
			putchar(w);
		} else {
//			violation(addr,0,4,"IO space access violation")
		}
	} else if (!band(addr,3)) {
        printf("Putting %d to RAM @ %d\n",band(w,4294967295),brshift(addr,2));
		RAM[brshift(addr,2)] = band(w,4294967295);
	} else {
		printf("WR violation @ 0x%08X: reminder is %u",addr,band(addr,3));
	}
}

void wr_word(int addr, int w)
{
	int r = get_dword(band(addr,4294967292));
	w = band(w,65535);
	if (!band(addr,2)) {
		wr_dword(band(addr,4294967292),bor(blshift(w,16),band(r,65535)));
	} else {
		wr_dword(band(addr,4294967292),bor(band(r,4294901760),w));
	}
}

void wr_byte(int addr, int d)
{
	int r = get_dword(band(addr,4294967292));
	int s = 8 * (3 - band(addr,3));
	int m = bxor(blshift(255,s),4294967295);
	r = bor(band(r,m),blshift(band(d,255),s));
	wr_dword(band(addr,4294967292),r);
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
		y = bor(y,band(x,2147483648));
		if (i < 31) { //don't shift last bit
			y = brshift(y,1);
			x = blshift(x,1);
		}
	}
	return y;
}

void im(int x)
{
	int v;
	if (idim != 0) {
		//next IM
		v = blshift(pop(),7);
		push(bor(v,x));
	} else {
		//first IM
		v = 0;
		int b = brshift(x,6);
		if (b != 0) {
			//propagate sign bit
			v = 4294967168;
		}
		push(bor(v,x));
	}
	idim = 1;
}

int sp_off(int w)
{
	return band((blshift(w,2) + SP),4294967295);
}

void iemu(char x)
{
	push(PC+1);
	PC = band((x * 32),4294967295);
	PChalt = 1;
}

void exec(int x)
{
    printf("PC = %d, SP = %d, Executing %d\n",PC,SP,x);
    getchar();
    
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
		push(bor(pop(),pop()));

	} else if (x == 6) { //ZPU_AND
		push(band(pop(),pop()));

	} else if (x == 8) { //ZPU_LOAD
		push(get_dword(pop()));

	} else if (x == 9) { //ZPU_NOT
		push(bxor(pop(),4294967295));

	} else if (x == 10) { //ZPU_FLIP
		push(flip(pop()));

	} else if (x == 12) { //ZPU_STORE
		int adr = pop();
		int val = pop();
		wr_dword(adr,val);

	} else if (x == 13) { //ZPU_POPSP
		SP = pop();

	} else if (x == 11) { //ZPU_NOP

	} else if (band(x,128)) { //ZPU_IM
		im(band(x,127));

	} else if (x >= 96) { //ZPU_LOADSP
		push(get_dword(sp_off(bxor(band(x,31),16))));

	} else if (x >= 64) { //ZPU_STORESP
		int a = sp_off(bxor(band(x,31),16));
        printf("StoreSP x = %d, (x&31) = %d, xor16 = %d, a = %d\n",x,band(x,31),bxor(band(x,31),16),a);
		wr_dword(a,pop());

	} else if (x >= 32) { //ZPU_EMULATE
		iemu(band(x,31));

	} else if (x >= 16) { //ZPU_ADDSP
		int a = sp_off(band(x,15));
		push(band((get_dword(a) + pop()),4294967295));

	} else {
		printf("Unknown instruction %hhu\n",x);
		halt = 1;
	}

	if (x < 128) {
		idim = 0;
	}
}

void step()
{
	if (halt) return;

    int n_addr = band(PC,4294967292);
	if (n_addr != i_addr) {
		i_addr = n_addr;
		i_hold = get_dword(i_addr);
		if (band(PC,3) > 0) {
			i_hold = band(blshift(i_hold,(8 * band(PC,3))),4294967295);
		}
	}

	printf("ihold = 0x%08X\n",i_hold);
	exec(brshift(i_hold,24));

	if (!PChalt) {
		PC = PC + 1;
		i_hold = band(blshift(i_hold,8),4294967295);
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
	SP = 2097152 - 8;
	PChalt = 0;
	idim = 0;
	i_addr = -1;
	i_hold = 0;

	// prepare global CPU state
	cycle = 0;
	halt = 0;
}

int main()
{
    int n = 0;
    puts("Hi there\n");
    for (int i = 0; i < blob_len/4; i++) {
        int x = 0;
        for (int j = 0; j < 4; j++) {
            x = bor(blshift(x,8),blob[n++]);
        }
        RAM[i] = x;
        //printf("Line %d (%d) out of %d copied\n",i,x,blob_len/4);
    }
    reset();
    n = 0;
    while (!halt) {
        step();
        if ((++n) % 10000 == 0) printf("%d\n",n);
    }
    puts("\nQuit.\n");
}
