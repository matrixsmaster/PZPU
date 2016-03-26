/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef AVR_CONFIG_H_
#define AVR_CONFIG_H_

//Enable debugging of internal functions
#define AVR_DBG 1

//Enable dry-run (without real execution)
#define AVR_DRYRUN 1

//16MHz internal clock (8 MHz RC + PLL)
#define F_CPU 16000000

//UART channel speed
#define UART_BAUDRATE 9600

//LCD module size (WxH) Set to zero to disable LCD output
#define LCD_SIZEW 16
#define LCD_SIZEH 2

//LCD strobe delay
#define LCD_STROBEDELAY_US	15

//LCD features enable
#define LCD_USE_RAM_FUNCTIONS
//#define LCD_USE_FLASH_FUNCTIONS
//#define LCD_USE_EEPROM_FUNCTIONS
//#define LCD_USE_CUSTOM_CHARACTER_FUNCTIONS

//Buttons config
#define BUTTON_LEFT		(PINC & (1<<PC5))
#define BUTTON_RIGHT	(PINC & (1<<PC3))
#define BUTTON_ENTER	(PINC & (1<<PC4))

#endif /* AVR_CONFIG_H_ */
