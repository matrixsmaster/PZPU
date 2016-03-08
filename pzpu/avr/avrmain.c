/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "avr_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "sd_raw.h"
#include "avr_io.h"
#include "../pzpu.h"
#include "../debug.h"

uint32_t img_offset,img_length;

int main(void)
{
	//Init serial interface (for terminal functions)
	USARTInit(UART_BAUDRATE);

#ifdef AVR_DBG
	USARTWriteString("Alive!"UART_LEND);
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
	USARTWriteString("Card connected"UART_LEND);
#endif

	//Read initial offset
	if ((!sd_raw_read(0,(uint8_t*)&img_offset,4)) || (!img_offset)) {
#ifdef AVR_DBG
		USARTWriteString("Error reading offset value!"UART_LEND);
#endif
		goto card_reset;
	}
	msg(0,"Image offset = 0x%08X"UART_LEND,img_offset);

	//Read image (memory) size
	if ((!sd_raw_read(4,(uint8_t*)&img_length,4)) || (!img_length)) {
#ifdef AVR_DBG
		USARTWriteString("Error reading length value!"UART_LEND);
#endif
		goto card_reset;
	}
	msg(0,"Image length = 0x%08X"UART_LEND,img_length);

	//TODO: RAM init

	//Main loop
#ifdef AVR_DBG
	USARTWriteString("Entering main loop..."UART_LEND);
#endif
	while (!status()) {
		step();
	}

	//End of user program execution
	sd_raw_sync();
	LED0_OFF;

	//Infinite loop
	for (;;) ;
}
