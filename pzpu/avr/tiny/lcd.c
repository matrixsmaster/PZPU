/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 * Ported from my old lcd_w595 library code.
 */

#include "lcd.h"
#include "soft_spi.h"
#include <util/delay.h>

uint8_t g_led;

//init sequence
const uint8_t LCDInitSeq[LCD_INITLEN] PROGMEM =
{ 0x02, 0x28, 0x01, 0x06, 0x0C };

void LCDSendNibble(const uint8_t dat, const LCDNibbleType typ)
{
	uint8_t out = 0;

	LCD_SHF_SET(out,LCD_RS,typ); //type
	LCD_SHF_SET(out,LCD_CE,1); //enable HIGH
	LCD_SHF_SET(out,LCD_D4,(dat & 1)); //data
	LCD_SHF_SET(out,LCD_D5,(dat & 2));
	LCD_SHF_SET(out,LCD_D6,(dat & 4));
	LCD_SHF_SET(out,LCD_D7,(dat & 8));
	out |= g_led; //preserve LED state

	spi(out);
	SPI_CLICK;

	LCD_DATA_DELAY;
	LCD_SHF_SET(out,LCD_CE,0); //enable LOW

	spi(out);
	SPI_CLICK;
}

void LCDSendByte(const uint8_t dat, const LCDNibbleType typ)
{
	SPI_PERIPH;
	LCDSendNibble(dat>>4,typ);
	LCDSendNibble(dat,typ);
}

void LCDClear(void)
{
	LCD_EXEC_DELAY;
	LCDSendByte(1,LCDCmd);
}

void LCDSetPos(const uint8_t col, const uint8_t row)
{
	LCD_EXEC_DELAY;
	LCDSendByte((0x80|((0x40*row)+col)),LCDCmd);
}

void LCDEnable(const LCDEnableState en)
{
	LCD_EXEC_DELAY;
	LCDSendByte((0x08|en),LCDCmd);
}

void LCDCursor(const LCDCursorKind cur)
{
	LCD_EXEC_DELAY;
	LCDSendByte((0x0C|cur),LCDCmd);
}

void LCDPuts(const char* s)
{
	if (!s) return;
	while (*s) {
		LCD_EXEC_DELAY;
		LCDSendByte(*s++,LCData);
	}
}

void LCD_Init(void)
{
	uint8_t i;
	//LCD_SHF_PREP;

	for (i = 0; i < LCD_INITLEN; i++) {
		LCD_EXEC_DELAY;
		LCDSendByte(pgm_read_byte(&(LCDInitSeq[i])),LCDCmd);
	}

	LCD_EXEC_DELAY;

	LCDEnable(LCDOn);
	LCDCursor(LCDCursorOff);

	LCD_EXEC_DELAY;
}

/*
inline void LCDBreak(void)
{
	SPI_PREAD; //dummy read to update CE pin of LCD
	SPI_CLICK; //latch it up
}
*/

inline static void printNibbleHex(const uint8_t b)
{
	LCD_EXEC_DELAY;
	if (b > 9) LCDSendByte(b-10+'A',LCData);
	else LCDSendByte(b+'0',LCData);
}

void LCDprintByteHex(const uint8_t b)
{
	printNibbleHex(b >> 4);
	printNibbleHex(b & 0xF);
}
