/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef AVR_IO_H_
#define AVR_IO_H_

#include <inttypes.h>
#include "avr_config.h"
#include <avr/io.h>

void USARTInit(const uint16_t baud);
char USARTReadChar(void);
char USARTReadIsReady(void);
void USARTWriteChar(const char data);
void USARTWriteString(const char* str);

extern uint32_t img_offset,img_length;

#endif /* AVR_IO_H_ */