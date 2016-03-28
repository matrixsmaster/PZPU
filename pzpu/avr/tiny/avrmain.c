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

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0;
	PORTB ^= (1<<PB4);
	tmr++;
}

int main(void)
{
	//Init serial interfaces
	spi_init();
	USARTInit(UART_BAUDRATE);

	TCCR1 = (1<<CS10)|(1<<CS11)|(1<<CS12)|(1<<CS13)|(1<<CTC1);
	TIMSK = (1<<OCIE1A);
	OCR1A = 255;
	DDRB |= (1<<PB4);

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

	sei();
	while (!status()) {
		step();
//		LCD_LEDTGL;
	}
	cli();

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

	LCDClear();
	LCDSetPos(0,1);
	LCDprintByteHex(tmr>>24);
	LCDprintByteHex(tmr>>16);
	LCDprintByteHex(tmr>>8);
	LCDprintByteHex(tmr);

	//Infinite sleep (until hard reset)
	sleep_enable();
	sleep_cpu();
}
