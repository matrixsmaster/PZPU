/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 * Ported from my old lcd_w595 library code.
 */

#ifndef LCD_H_
#define LCD_H_

#include <inttypes.h>
#include <avr/pgmspace.h>

//small delay (data latching)
#define LCD_DATA_DELAY _delay_us(15);
//big delay (command execution)
#define LCD_EXEC_DELAY _delay_ms(1);

//bit setting macros
#define TO_BOOL(X) ((X)?1:0)
#define LCD_SHF_SET(X,T,B) { X &= ~(1<<T); X |= (TO_BOOL(B)<<T); }

//LED
#define LCD_LEDON { g_led = (1<<LCD_LED); SPI_PREAD; SPI_CLICK; }
#define LCD_LEDOFF { g_led = 0; SPI_PREAD; SPI_CLICK; }
#define LCD_LEDTGL { g_led ^= (1<<LCD_LED); SPI_PREAD; SPI_CLICK; }

//init sequence length
#define LCD_INITLEN 5

//Connections between 595 register and LCD+LED
typedef enum {
	LCD_LED = 0,
	LCD_RS,
	LCD_RW,
	LCD_CE,
	LCD_D4,
	LCD_D5,
	LCD_D6,
	LCD_D7
} LCDBits;

//Data types for LCD control
typedef enum {
	LCDCmd = 0,
	LCData
} LCDNibbleType;

typedef enum {
	LCDOff = 0,
	LCDOn = 4
} LCDEnableState;

typedef enum {
	LCDCursorOff = 0,
	LCDCursorOn = 2,
	LCDCursorBlink = 3
} LCDCursorKind;

//Prototypes
void LCDSendByte(const uint8_t dat, const LCDNibbleType typ);
void LCDClear(void);
void LCDSetPos(const uint8_t col, const uint8_t row);
void LCDEnable(const LCDEnableState en);
void LCDCursor(const LCDCursorKind cur);
void LCDPuts(const char* s);
void LCD_Init(void); //to not to confuse with top-level LCDInit() function in avr_io
void LCDprintByteHex(const uint8_t b);

#endif /* LCD_H_ */
