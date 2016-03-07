/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdarg.h>
#include <stdio.h>
#include "debug.h"

void msg(uint8_t err, const char* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);

#ifdef DEBUG_OS_ENABLED
	vfprintf((err? stderr:stdout),fmt,vl);
#endif

	va_end(vl);
}
