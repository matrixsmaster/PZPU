/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef AVR_CONFIG_H_
#define AVR_CONFIG_H_

//Enable debugging of internal functions
#define AVR_DBG 1

//Enable dry-run (without real execution)
//#define AVR_DRYRUN 1

//20MHz external crystal
#define F_CPU 20000000

//UART channel speed
#define UART_BAUDRATE 9600

//UART channel line ending
//#define UART_LEND "\r\n"

//Enable time measurement
#define AVR_TIME 1

//LED0 config (PB0)
#define LED0_ON { DDRB |= 1; PORTB |= 1; }
#define LED0_OFF { DDRB |= 1; PORTB &= ~1; }

#endif /* AVR_CONFIG_H_ */
