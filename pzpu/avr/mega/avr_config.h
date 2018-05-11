/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef AVR_CONFIG_H_
#define AVR_CONFIG_H_

//Enable debugging of internal functions
//#define AVR_DBG 1

//Enable dry-run (without real execution)
//#define AVR_DRYRUN 1

//25MHz external crystal
#define F_CPU 25000000

//UART channel speed
#define UART_BAUDRATE 57600

//Enable time measurement
#define AVR_TIME 1

//Enable user-initiated start
#define AVR_WAITFORSTART 1

//LED0 config
#define LED0			(1<<PB1)
#define LED0_ON			{ DDRB |= LED0; PORTB |= LED0; }
#define LED0_OFF		{ DDRB |= LED0; PORTB &= ~LED0; }
#define LED0_TGGL		PORTB ^= LED0

//LCD module size (WxH) Set to zero to disable LCD output
#define LCD_SIZEW 12
#define LCD_SIZEH 2
#define LCD_HELLO "Sofy rev.A3"

//LCD pin map: data (4-bit interface D4-D7)
#define LCDDATAPORT		PORTD
#define LCDDATADDR		DDRD
#define LCDDATAPIN		PIND
#define LCD_D4			2
#define LCD_D5			3
#define LCD_D6			4
#define LCD_D7			5

//LCD pin map: control
#define LCDCONTROLPORT	PORTC
#define LCDCONTROLDDR	DDRC
#define LCD_RS			0
#define LCD_RW			1
#define LCD_E			2

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
