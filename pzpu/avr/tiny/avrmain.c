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
volatile uint32_t tmr = 0;

#ifdef AVR_DBG
static int freeRam()
{
	//From http://jeelabs.org/2011/05/22/atmega-memory-use/
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif /* AVR_DBG */

#ifdef AVR_TIME
ISR(TIMER0_COMPA_vect)
{
	tmr++;
}
#endif

int main(void)
{
	//Init serial interfaces
	spi_init();
//	USARTInit(UART_BAUDRATE);

#ifdef AVR_TIME
	TCCR0A = (1<<WGM01); //CTC
	TCCR0B = (1<<CS02)|(1<<CS00); //Clk/1024
	TIMSK = (1<<OCIE0A);
	OCR0A = 252; //62 Hz interrupts
#endif

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
#ifdef AVR_DBG
	USARTWriteChar('B');
#endif

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

	sei();
	while (!status()) {
		step();
	}
	cli();

#ifdef AVR_DBG
	USARTWriteChar('E');
#endif

	//End of user program execution
	ram_release(); //sync and disconnect RAM card
	LCD_LEDOFF;

#ifdef AVR_DBG
	LCDClear();
	LCDSetPos(0,0);

	int fr = freeRam();
	LCDPuts("RAM:");
	LCDprintByteHex(fr>>8,0);
	LCDprintByteHex(fr,0);

#ifdef AVR_TIME
	LCDSetPos(0,1);
	LCDPuts("Time");
	LCDprintByteHex(tmr>>24,0);
	LCDprintByteHex(tmr>>16,0);
	LCDprintByteHex(tmr>>8,0);
	LCDprintByteHex(tmr,0);
#endif
#endif

	//Infinite sleep (until hard reset)
	sleep_enable();
	sleep_cpu();
}
