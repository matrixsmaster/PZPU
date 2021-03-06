/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef AVR_IO_H_
#define AVR_IO_H_

#include <inttypes.h>
#include <avr/io.h>

void USARTInit(const uint32_t baud);
char USARTReadChar(void);
char USARTReadIsReady(void);
void USARTWriteChar(const char data);
void USARTWriteString(const char* str);

void LCDInit(void);
void LCDWriteChar(const char data);
void LCDprintByteHex(const uint8_t b, const uint8_t curs);

#endif /* AVR_IO_H_ */
