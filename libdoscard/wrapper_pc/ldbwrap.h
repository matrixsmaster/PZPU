/*
 *  Copyright (C) 2014  Soloviov Dmitry
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef LDBWRAP_H_
#define LDBWRAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <string>
#include "dosbox.h"
#include "ldbconf.h"
#include "ldb.h"

namespace doscard {

typedef struct {
	bool on;
	uint16_t lcdw,lcdh;
	uint64_t sndsize;
} LDBI_RuntimeData;

typedef vector<dosbox::LDB_UIEvent> LDBI_EventVec;
typedef vector<string> LDBI_MesgVec;

int LDBCB_LCD(void* buf, size_t len);
int LDBCB_SND(void* buf, size_t len);
int LDBCB_UIE(void* buf, size_t len);
int LDBCB_TCK(void* buf, size_t len);
int LDBCB_MSG(void* buf, size_t len);
int LDBCB_FIO(void* buf, size_t len);

extern dosbox::CDosBox* DOS;
extern LDBI_RuntimeData* Runtime;
extern uint32_t* Screen;
extern int16_t* Sound;
extern LDBI_EventVec Events;
extern LDBI_MesgVec Messages;

int CreateInstance(dosbox::LDB_Settings* set);
int TryDestroyInstance(void);
int RunInstance(void);
int GetInstanceRuntime(void*,uint64_t);
int GetInstanceScreen(void*,uint64_t);
int GetInstanceSound(void*,uint64_t);
int AddInstanceEvents(void*,uint64_t);
int GetInstanceMessages(void*,uint64_t);


} //namespace doscard

#endif /* LDBWRAP_H_ */