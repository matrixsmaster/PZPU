/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef IO_H_
#define IO_H_

#include <inttypes.h>

//Enable debug output
#define PZPU_IODBG 1

//Enable interactive mode for UART
#define PZPU_IOINTERACT 1

//Emulated board IO offset
#define EBOARD_OFFSET 0x080A0000U

//Address table
enum EBoardIO {
	BZPU_ZPUEn = 0x0,
	BZPU_GPIOdata = 0x4,
	BZPU_GPIOdir = 0x8,
	BZPU_UARTTx = 0xC,
	BZPU_UARTRx = 0x10,
	BZPU_CntL = 0x14,
	BZPU_CntH = 0x18,
	BZPU_INTmask = 0x20,
	BZPU_UARTINTEn = 0x24,
	BZPU_UARTINT = 0x28,
	BZPU_TmrINTEn = 0x2C,
	BZPU_TmrINT = 0x30,
	BZPU_TmrPeriod = 0x34,
	BZPU_TmrCount = 0x38,
	BZPU_MHZ = 0x3C
};

//Emulator IO function prototypes
void io_wr(uint32_t adr, uint32_t be);
uint32_t io_rd(uint32_t adr);

//Host-specific functions (implemented outside io.c module)
void term_putchar(const uint32_t x);
uint32_t term_getchar(void);

#endif /* IO_H_ */
