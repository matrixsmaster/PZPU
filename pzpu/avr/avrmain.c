/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "avr_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sd_raw.h"
#include "avr_io.h"
#include "../pfmt.h"
#include "../pzpu.h"
#include "../ram.h"
#include "../debug.h"

uint32_t img_offset,img_length;

#ifdef AVR_TIME
volatile uint32_t chip_time;

ISR(TIMER1_COMPA_vect)
{
	chip_time++;
//	LED0_TGGL;
}
#endif

int main(void)
{
	//Init timer
#ifdef AVR_TIME
	chip_time = 0;
	TIMSK1 = (1<<OCIE1A);
	TCCR1A = 0;

#if (F_CPU==20000000)
	TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
	OCR1A = 9765U; //2Hz
#else
#error "Please correct the timer values for different CPU speed!"
#endif /* speed check */
#endif /* AVR_TIME */

	//Init serial interface (for terminal functions)
	USARTInit(UART_BAUDRATE);

	//Enable interrupts
	sei();

#ifdef AVR_DBG
	USARTWriteString("Alive!\n");
	msg(0,"int = %d\n",sizeof(int));
	msg(0,"long = %d\n",sizeof(long));
	msg(0,"long long = %d\n",sizeof(long long));
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
	USARTWriteString("Card connected\n");
#endif

	//Read initial offset
	if ((!sd_raw_read(0,(uint8_t*)&img_offset,4)) || (!img_offset)) {
#ifdef AVR_DBG
		USARTWriteString("Error reading offset value!\n");
#endif
		goto card_reset;
	}
	msg(0,"Image offset = 0x%08lX\n",img_offset);

	//Read image (memory) size
	if ((!sd_raw_read(4,(uint8_t*)&img_length,4)) || (!img_length)) {
#ifdef AVR_DBG
		USARTWriteString("Error reading length value!\n");
#endif
		goto card_reset;
	}
	msg(0,"Image length = 0x%08lX\n",img_length);

	//Try to initialize our virtual RAM
	if (ram_init(img_length)) {
#ifdef AVR_DBG
		msg(1,"Error initializing RAM! Code = %hu\n",ram_init(img_length));
#endif
		goto card_reset;
	}
	msg(0,"RAM init complete.\n");

	//Everything is ready. Reset PZPU
#ifndef AVR_DRYRUN
	reset(img_length);
#ifdef AVR_DBG
	USARTWriteString("PZPU reset complete.\n");
#endif
#endif

	//Main loop
#ifdef AVR_DBG
	USARTWriteString("Entering main loop...\n");
#endif
	while (!status()) {
		step();
		LED0_TGGL;
//		USARTWriteChar('.');
//		USARTReadChar(); //step-by-step
//		msg(0,"CC=%08lX\r\n",get_cycles(0));
	}
#ifdef AVR_DBG
	USARTWriteString("End of main loop.\n");
#endif
	msg(0,"Cycles = 0x"PFMT_32XINT PFMT_32XINT"\n",get_cycles(1),get_cycles(0));

	//End of user program execution
	ram_release(); //sync and disconnect RAM card
	cli(); //we don't need interrupts anymore
	LED0_OFF;

#ifdef AVR_DBG
	USARTWriteString("Emulator halted.\n");
#endif

#ifdef AVR_TIME
	msg(0,"Time passed = "PFMT_32UINT" 2xSec\n",chip_time);
	if (chip_time) {
		uint64_t _cc = get_cycles(1);
		_cc <<= 32;
		_cc |= get_cycles(0);
		uint16_t _ch = chip_time >> 1;
		_ch = _cc / _ch;
		msg(0,"Execution speed (med) = "PFMT_16UINT" inst/sec\n",_ch);
	}
#endif

	//Infinite loop
	for (;;) ; //TODO: put CPU into sleep mode
}
