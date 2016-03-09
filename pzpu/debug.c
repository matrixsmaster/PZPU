/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdarg.h>
#include <stdio.h>
#include "debug.h"

#if EMBED_AVR
#include "avr/avr_io.h"
#endif

void msg(uint8_t err, const char* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);

#if DEBUG_OS_ENABLED
	vfprintf((err? stderr:stdout),fmt,vl);

#elif EMBED_AVR
	char str[DEBUG_MAXMSGLEN];
	vsnprintf(str,sizeof(str),fmt,vl);
	if (err) USARTWriteString("ERR: ");
	USARTWriteString(str);

#endif

	va_end(vl);
}
