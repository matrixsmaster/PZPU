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
#include "debug.h"

uint32_t img_offset,img_length;

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
	//Init serial interface (for terminal functions)
	USARTInit(UART_BAUDRATE);

	//Init LCD display
#if LCD_SIZEW && LCD_SIZEH
	LCDInit();
#endif

	//Init SD card interface
card_reset:
	img_offset = 0;
	img_length = 0;

	LED0_OFF;
	while (!sd_raw_init()) {
		_delay_ms(1000);
	}
	LED0_ON;

#ifdef AVR_DBG
	USARTWriteChar('A');
#endif

	//Read initial offset
	if ((!sd_raw_read(0,(uint8_t*)&img_offset,4)) || (!img_offset)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('1');
#endif
		goto card_reset;
	}
//	msg(0,"Image offset = 0x%08lX\n",img_offset);

	//Read image (memory) size
	if ((!sd_raw_read(4,(uint8_t*)&img_length,4)) || (!img_length)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('2');
#endif
		goto card_reset;
	}
//	msg(0,"Image length = 0x%08lX\n",img_length);

	//Try to initialize our virtual RAM
	if (ram_init(img_length)) {
#ifdef AVR_DBG
		USARTWriteChar('E');
		USARTWriteChar('3');
		USARTWriteChar(ram_init(img_length));
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
		LED0_TGGL;
	}
#ifdef AVR_DBG
	USARTWriteChar('E');
#endif
//	msg(0,"Cycles = 0x"PFMT_32XINT PFMT_32XINT"\n",get_cycles(1),get_cycles(0));

	//End of user program execution
	ram_release(); //sync and disconnect RAM card
	LED0_OFF;

#ifdef AVR_DBG
//	msg(0,"Free RAM: %d bytes\n",freeRam());
	USARTWriteChar('F');
#endif

	//Infinite sleep (until hard reset)
	sleep_enable();
	sleep_cpu();
}
