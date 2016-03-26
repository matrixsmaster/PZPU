/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef SOFT_SPI_H_
#define SOFT_SPI_H_

#include <avr/io.h>
#include <inttypes.h>
#include "avr_config.h"

#define SPI_CARD	SPIPORT &= ~(1<<PIN_SS);
#define SPI_PERIPH	SPIPORT |= (1<<PIN_SS);
#define SPI_CLICK 	{ SPI_CARD SPI_PERIPH }
#define SPI_PREAD	spi(0)

uint8_t spi(uint8_t x);
void spi_init(void);

#endif /* SOFT_SPI_H_ */
