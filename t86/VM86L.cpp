// 8086tiny: a tiny, highly functional, highly portable PC emulator/VM
// Copyright 2013-14, Adrian Cable (adrian.cable@gmail.com) - http://www.megalith.co.uk/8086tiny
//
// Revision 1.25
//
// Changed by Dmitry 'MatrixS_Master' Soloviov, 2015-2016
//
// This work is licensed under the MIT License. See included LICENSE.TXT.

#include <unistd.h>
#include <fcntl.h>
#include "VM86.h"

#ifndef USE_RAW_OUTPUT
#include <stdio.h>
#include <ctype.h>
#endif

#ifdef MRAM_TEST
void my_memcpy(RAMptr<uch>* p, const void* src, unsigned len)
{
	unsigned i;
	const uch* ptr = (const uch*)src;
	for (i = 0; i < len; i++)
		(*p)[i] = ptr[i];
	printf("memcpy(): %u bytes copied\n",i);
}

ssize_t my_read(int id, RAMptr<uch>* p, size_t n)
{
	unsigned i;
	uch tmp;
	for (i = 0; i < n; ) {
		if (read(id,&tmp,1) == 1) (*p)[i++] = tmp;
		else break;
	}
	return i;
}

ssize_t my_write(int id, RAMptr<uch>* p, size_t n)
{
	unsigned i;
	for (i = 0; i < n; ) {
		if (read(id,&((*p)[i++]),1) != 1) break;
	}
	return i;
}
#endif

void VM86::LocalOpcode()
{
#ifdef MRAM_TEST
	RAMptr<uch> tmp(&mem,SEGREG(REG_ES, REG_BX,));
#else
	unsigned char* tmp = mem + SEGREG(REG_ES, REG_BX,);
#endif

	switch ((char)i_data0)
	{
		OPCODE_CHAIN 0: // PUTCHAR_AL
#ifndef USE_RAW_OUTPUT
			if ((*regs8 == 10 || *regs8 == 13) || isprint(*regs8))
				putchar(*regs8);
			else
				printf("byte out: %hhu\n",*regs8);
#else
			write(1, regs8, 1);
#endif

		OPCODE 1: // GET_RTC
			time(&clock_buf);
			ftime(&ms_clock);
#ifdef MRAM_TEST
			my_memcpy(&tmp, localtime(&clock_buf), sizeof(struct tm));
#else
			memcpy(tmp, localtime(&clock_buf), sizeof(struct tm));
#endif
			CAST(short)mem[SEGREG(REG_ES, REG_BX, 36+)] = ms_clock.millitm;

		OPCODE 2: // DISK_READ
			regs8[REG_AL] = ~lseek(disk[regs8[REG_DL]], CAST(unsigned)regs16[REG_BP] << 9, 0)
#ifdef MRAM_TEST
				? (int)my_read(disk[regs8[REG_DL]], &tmp, regs16[REG_AX])
#else
				? (int)read(disk[regs8[REG_DL]], tmp, regs16[REG_AX])
#endif
				: 0;

		OPCODE 3: // DISK_WRITE
			regs8[REG_AL] = ~lseek(disk[regs8[REG_DL]], CAST(unsigned)regs16[REG_BP] << 9, 0)
#ifdef MRAM_TEST
				? (int)my_write(disk[regs8[REG_DL]], &tmp, regs16[REG_AX])
#else
				? (int)write(disk[regs8[REG_DL]], tmp, regs16[REG_AX])
#endif
				: 0;
	}
}
