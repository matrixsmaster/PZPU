/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <inttypes.h>

//Enable OS terminal IO functions
#define DEBUG_OS_ENABLED 1

//Max message length for embedded devices
#define DEBUG_MAXMSGLEN 56

void msg(uint8_t err, const char* fmt, ...);


#endif /* DEBUG_H_ */
