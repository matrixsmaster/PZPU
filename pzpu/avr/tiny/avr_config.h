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

//16MHz internal clock (8 MHz RC + PLL)
#define F_CPU 16000000

//UART channel speed
#define UART_BAUDRATE 9600

//SPI channel configuration
#define SPIPORT		PORTB
#define SPIDDR		DDRB
#define SPIPIN		PINB
#define PIN_DI		PB0
#define PIN_DO		PB1
#define PIN_SCK		PB4
#define PIN_SS		PB3

//LCD module size (WxH) Set to zero to disable LCD output
#define LCD_SIZEW 12
#define LCD_SIZEH 2

//LCD features enable
#define LCD_USE_RAM_FUNCTIONS
//#define LCD_USE_FLASH_FUNCTIONS
//#define LCD_USE_EEPROM_FUNCTIONS
//#define LCD_USE_CUSTOM_CHARACTER_FUNCTIONS

#endif /* AVR_CONFIG_H_ */
