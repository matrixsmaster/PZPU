/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "avr_config.h"
#include "soft_spi.h"

//will take approx. 12-14 cycles with -O2
uint8_t spi(uint8_t x)
{
	uint8_t i;
	for (i = 0; i < 8; i++) {
		SPIPORT &= ~(1<<PIN_SCK);
		if (x & 0x80) SPIPORT |= (1<<PIN_DO);
		else SPIPORT &= ~(1<<PIN_DO);
		SPIPORT |= (1<<PIN_SCK);
		x <<= 1;
		if (SPIPIN & (1<<PIN_DI)) x |= 1;
	}
	SPIPORT &= ~(1<<PIN_SCK);
	return x;
}

void spi_init(void)
{
	SPIDDR  |= (1<<PIN_DO)|(1<<PIN_SCK)|(1<<PIN_SS);
	SPIPORT |= (1<<PIN_DI); //DI pullup

	SPI_PERIPH; //select peripherals
}
