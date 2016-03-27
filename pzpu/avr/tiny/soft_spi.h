/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef SOFT_SPI_H_
#define SOFT_SPI_H_

#include <avr/io.h>
#include <inttypes.h>
#include "avr_config.h"

//SPI bitbang helpers
#define SPI_CARD	SPIPORT &= ~(1<<PIN_SS);
#define SPI_PERIPH	SPIPORT |= (1<<PIN_SS);
#define SPI_CLICK 	{ SPI_CARD SPI_PERIPH }
#define SPI_PREAD	spi(g_led) /* Zeroes on all LCD lines; don't touch LED state */

//LED state global variable
extern uint8_t g_led;

//Software SPI function prototypes
uint8_t spi(uint8_t x);
void spi_init(void);

#endif /* SOFT_SPI_H_ */
