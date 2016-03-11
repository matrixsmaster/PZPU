/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <string.h>
#include "avr_io.h"
#include "lcd-library.h"

static uint8_t lcd_px,lcd_py;

void USARTInit(const uint16_t baud)
{
	uint16_t val = ((uint32_t)F_CPU / baud / 16) - 1;
    UBRR0L = val & 0xff;
    UBRR0H = val >> 8;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

inline char USARTReadChar(void)
{
    while (!(UCSR0A & (1<<RXC0))) ;
    return UDR0;
}

inline char USARTReadIsReady(void)
{
    if (!(UCSR0A & (1<<RXC0))) return 0;
    return 1;
}

inline void USARTWriteChar(const char data)
{
    while (!(UCSR0A & (1<<UDRE0))) ;
    UDR0=data;
}

void USARTWriteString(const char* str)
{
    unsigned int i = 0;
    while (str[i] != '\0')
        USARTWriteChar(str[i++]);
}

void LCDInit(void)
{
	lcdInit();
	lcdClear();
	lcdSetCursor(LCD_CURSOR_BLINK);
	lcd_px = lcd_py = 0;
}

void LCDWriteChar(const char data)
{
	if (++lcd_px > LCD_SIZEW) {
		lcd_px = 0;
		if (++lcd_py > LCD_SIZEH) {
			lcd_py = 0;
		}
		lcdGotoXY(lcd_py,lcd_px);
	}
	while (lcdIsBusy()) ;
	lcdRawSendByte(data,LCD_DATA);
}

inline uint32_t swap(uint32_t x)
{
	uint32_t y = (x & 0xff);
	y <<= 8; x >>= 8;
	y |= (x & 0xff);
	y <<= 8; x >>= 8;
	y |= (x & 0xff);
	y <<= 8; x >>= 8;
	y |= x;
    return y;
}
