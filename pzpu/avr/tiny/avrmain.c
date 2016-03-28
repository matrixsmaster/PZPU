/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "avr_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "sd_raw.h"
#include "avr_io.h"
#include "pfmt.h"
#include "pzpu.h"
#include "ram.h"
#include "lcd.h"
#include "soft_spi.h"
#include "debug.h"

uint32_t img_offset,img_length,img_blk_offset;

//extern uint32_t sp,pc;

#ifdef AVR_DBG
static int freeRam()
{
	//From http://jeelabs.org/2011/05/22/atmega-memory-use/
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif /* AVR_DBG */

int main(void)
{
	//Init serial interfaces
	spi_init();
	USARTInit(UART_BAUDRATE);

	//Init LCD display
#if LCD_SIZEW && LCD_SIZEH
	LCDInit();
#endif

	//Init SD card interface
card_reset:
	img_offset = 0;
	img_length = 0;
	img_blk_offset = 0;

	LCD_LEDOFF;
	while (!sd_raw_init()) {
		_delay_ms(1000);
	}
	LCD_LEDON;

#ifdef AVR_DBG
	USARTWriteChar('A');
#endif

	//Read initial offset
	if (!sd_raw_read(0,(uint8_t*)&img_offset,4)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('1');
#endif
		goto card_reset;
	}

	//Read image (memory) size
	if ((!sd_raw_read(4,(uint8_t*)&img_length,4)) || (!img_length)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('2');
#endif
		goto card_reset;
	}

	//Read blockwise offset
	if (!sd_raw_read(8,(uint8_t*)&img_blk_offset,4)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('3');
#endif
		goto card_reset;
	}

	//Try to initialize our virtual RAM
	if (ram_init(img_length)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('>');
		USARTWriteChar(ram_init(img_length)+'0');
#endif
		goto card_reset;
	}
	USARTWriteChar('B');

	//Everything is ready. Reset PZPU
#ifndef AVR_DRYRUN
	reset(img_length);
#ifdef AVR_DBG
	USARTWriteChar('C');
#endif
#endif

	//Main loop
#ifdef AVR_DBG
	USARTWriteString("D");
#endif
	while (!status()) {
		step();
		LCD_LEDTGL;
	}
#ifdef AVR_DBG
	USARTWriteChar('E');
#endif
//	msg(0,"Cycles = 0x"PFMT_32XINT PFMT_32XINT"\n",get_cycles(1),get_cycles(0));

	//End of user program execution
	ram_release(); //sync and disconnect RAM card
	LCD_LEDOFF;

#ifdef AVR_DBG
//	msg(0,"Free RAM: %d bytes\n",freeRam());
	USARTWriteChar('F');
#endif

//	LCDSetPos(0,0);
//	LCDprintByteHex(pc>>24);
//	LCDprintByteHex(pc>>16);
//	LCDprintByteHex(pc>>8);
//	LCDprintByteHex(pc);
//	LCDprintByteHex(ram_rd_b(0x348));
//	LCDprintByteHex(ram_rd_b(0x349));
//	LCDprintByteHex(ram_rd_b(0x34a));
//	LCDprintByteHex(ram_rd_b(0x34b));
//	LCDprintByteHex(ram_rd_b(0x34c));
//	LCDprintByteHex(ram_rd_b(0x34d));

//	LCDSetPos(0,1);
//	LCDprintByteHex(sp>>24);
//	LCDprintByteHex(sp>>16);
//	LCDprintByteHex(sp>>8);
//	LCDprintByteHex(sp);
//	LCDprintByteHex(get_cycles(0)>>24);
//	LCDprintByteHex(get_cycles(0)>>16);
//	LCDprintByteHex(get_cycles(0)>>8);
//	LCDprintByteHex(get_cycles(0));
//	LCDprintByteHex(ram_rd_b(0x0));
//	LCDprintByteHex(ram_rd_b(0x1));
//	LCDprintByteHex(ram_rd_b(0x2));
//	LCDprintByteHex(ram_rd_b(0x3));
//	LCDprintByteHex(ram_rd_b(0x4));
//	LCDprintByteHex(ram_rd_b(0x5));

	//Infinite sleep (until hard reset)
	sleep_enable();
	sleep_cpu();
}
