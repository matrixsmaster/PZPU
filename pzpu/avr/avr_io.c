/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <string.h>
#include "avr_io.h"

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

inline uint32_t swap(uint32_t x)
{
	//FIXME: need more speedy solution
    uint8_t a[4],temp;
    memcpy (a,&x,4);
    temp = a[0];
    a[0] = a[3];
    a[3] = temp;
    temp = a[1];
    a[1] = a[2];
    a[2] = temp;
    memcpy (&x,a,4);
    return x;
}
