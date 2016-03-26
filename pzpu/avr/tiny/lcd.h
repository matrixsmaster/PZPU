/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 * Ported from my old lcd_w595 library code.
 */

#ifndef LCD_H_
#define LCD_H_

#include <inttypes.h>

//small delay (data latching)
#define LCD_DATA_DELAY _delay_us(15);
//big delay (command execution)
#define LCD_EXEC_DELAY _delay_ms(1);

#define TO_BOOL(X) ((X)?1:0)
#define LCD_SHF_SET(X,T,B) { X &= ~(1<<T); X |= (TO_BOOL(B)<<T); }

//init sequence
#define LCD_INITLEN 5
//FIXME: PROGMEM!
static const uint8_t LCDInitSeq[LCD_INITLEN] =
{ 0x02, 0x28, 0x01, 0x06, 0x0C };

typedef enum {
	LCD_RS = 1,
	LCD_RW,
	LCD_CE,
	LCD_D4,
	LCD_D5,
	LCD_D6,
	LCD_D7
} LCDBits;

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

void LCDSendByte(const uint8_t dat, const LCDNibbleType typ);
void LCDClear(void);
void LCDSetPos(const uint8_t col, const uint8_t row);
void LCDEnable(const LCDEnableState en);
void LCDCursor(const LCDCursorKind cur);
void LCDPuts(const char* s);
void LCD_Init(void); //to not to confuse with top-level LCDInit() function in avr_io

#endif /* LCD_H_ */
