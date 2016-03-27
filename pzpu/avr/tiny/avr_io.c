/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include "avr_config.h"
#include <util/delay.h>
#include <string.h>
#include "avr_io.h"
#include "lcd.h"

static uint8_t lcd_px,lcd_py;

void USARTInit(const uint32_t baud)
{
}

inline char USARTReadChar(void)
{
	return 0;
}

inline char USARTReadIsReady(void)
{
	return 1;
}

inline void USARTWriteChar(const char data)
{
	LCDWriteChar(data);
}

void USARTWriteString(const char* str)
{
    unsigned int i = 0;
    while (str[i] != '\0')
        USARTWriteChar(str[i++]);
}

void LCDInit(void)
{
	LCD_Init();
	LCDClear();
	LCDCursor(LCDCursorBlink);
	lcd_px = lcd_py = 0;
}

void LCDWriteChar(const char data)
{
	if (++lcd_px > LCD_SIZEW) {
		lcd_px = 0;
		if (++lcd_py > LCD_SIZEH) {
			lcd_py = 0;
		}
		LCDSetPos(lcd_px,lcd_py);
	}
	LCDSendByte(data,LCData);
}

inline static void printNibbleHex(const uint8_t b)
{
	LCD_EXEC_DELAY;
	if (b > 9) LCDSendByte(b-10+'A',LCData);
	else LCDSendByte(b+'0',LCData);
}

void printByteHex(const uint8_t b)
{
	printNibbleHex(b >> 4);
	printNibbleHex(b & 0xF);
}
