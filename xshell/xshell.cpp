/*
 *  Copyright (C) 2013-2014  Soloviov Dmitry
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

#include "xshell.h"
#include "xsupport.h"

/*
 * The Basic model:
 * 1. Run SDL window
 * 2. Start Box thread
 * 2.a. Box thread calls UpdateScreenBuffer() every it's own frame
 * 2.b. Box thread calls UpdateSoundBuffer() every time the buffer is ready
 * 2.c. Box thread calls QueryUIEvents() every time the thread ready to process them
 * 2.note. This mechanism is close enough to what will happen on a real chip
 * (video generator may be a separate chip, which connected to main chip via UART/SPI/I2C
 * or may be just a timer's interrupt handler; audio output controller may be implemented
 * as simple timer interrupt, which updates the other timer's values to change duty
 * cycle of PWM generated; keyboard handler is a pin change interrupt handler too;
 * all of this parts will never directly affect the main code)
 *
 * Excessive debug output needed for future stress tests and profiling. So better I'll
 * write it now :)
 */

SDL_Window *wnd = NULL;
SDL_Renderer *ren = NULL;
struct timespec* clkres = NULL;
uint32_t* clkbeg = NULL;

int XS_UpdateScreenBuffer(void* buf, size_t len)
{
#ifdef XSHELL_VERBOSE
	xnfo(0,2,"len=%d",len);
#endif
	return 0;
}

int XS_UpdateSoundBuffer(void* buf, size_t len)
{
#ifdef XSHELL_VERBOSE
	xnfo(0,3,"len=%d",len);
#endif
	return 0;
}

int XS_QueryUIEvents(void* buf, size_t len)
{
#ifdef XSHELL_VERBOSE
	xnfo(0,4,"len=%d",len);
#endif
	return 0;
}

int XS_GetTicks(void* buf, size_t len)
{
	uint32_t* val = reinterpret_cast<uint32_t*>(buf);
	struct timespec r;
#ifdef XSHELL_VERBOSE
	xnfo(0,5,"len=%d",len);
#endif
	if ((!buf) || (len < 4)) return -1;
	if (!clkres) {
		clkres = new struct timespec;
		clock_getres(CLOCK_MONOTONIC,clkres);
		xnfo(0,5,"Resolution is %d:%d",clkres->tv_sec,clkres->tv_nsec);
		if (clkres->tv_nsec > 1000000)
			xnfo(-1,5,"CLOCK_MONOTONIC resolution is too low!");
	}
	clock_gettime(CLOCK_MONOTONIC,&r);
	if (!clkbeg) {
		clkbeg = new uint32_t;
		xnfo(0,5,"Clocks start point set to %d:%d",r.tv_sec,r.tv_nsec);
		*clkbeg = r.tv_sec * 1000 + (r.tv_nsec / 1000000 / clkres->tv_nsec);
		xnfo(0,5,"|-> %d ms",*clkbeg);
		*val = 0;
	} else {
		*val = r.tv_sec * 1000 + (r.tv_nsec / 1000000 / clkres->tv_nsec);
		*val -= *clkbeg;
	}
	return 0;
}

static void XS_ldb_register()
{
	Dosbox_RegisterCallback(DBCB_GetTicks,&XS_GetTicks);
	Dosbox_RegisterCallback(DBCB_PushScreen,&XS_UpdateScreenBuffer);
	Dosbox_RegisterCallback(DBCB_PushSound,&XS_UpdateSoundBuffer);
	Dosbox_RegisterCallback(DBCB_PullUIEvents,&XS_QueryUIEvents);
	Dosbox_RegisterCallback(DBCB_PushMessage,&XS_Message);
#ifdef XSHELL_VERBOSE
	xnfo(0,6,"finished");
#endif
}

static int XS_SDLInit()
{
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		xnfo(1,7,"SDL2 Init Error");
		return 1;
	}
	if (SDL_CreateWindowAndRenderer(XSHELL_DEF_WND_W,XSHELL_DEF_WND_H,
			SDL_WINDOW_OPENGL,&wnd,&ren)) {
		xnfo(1,7,"CreateWindowAndRenderer() failed.");
		return 2;
	}
	if (SDL_SetRenderDrawColor(ren,0,0,0,255)) {
		xnfo(1,7,"SetRenderDrawColor() failed.");
		return 10;
	}
	SDL_SetWindowTitle(wnd,XSHELL_CAPTION);
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);
#ifdef XSHELL_VERBOSE
	xnfo(0,7,"Init OK");
#endif
	return 0;
}

static void XS_SDLKill()
{
#ifdef XSHELL_VERBOSE
	xnfo(0,8,"killing...");
#endif
	if (ren) SDL_DestroyRenderer(ren);
	if (wnd) SDL_DestroyWindow(wnd);
	SDL_Quit();
}

static void XS_SDLoop()
{
	SDL_Event e;
	xnfo(0,9,"Loop begins");
	for(;;) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) return;
		}
		SDL_RenderClear(ren);
		SDL_RenderPresent(ren);
		SDL_Delay(5);
	}
}

int XS_Message(void* buf, size_t len)
{
#ifdef XSHELL_VERBOSE
	xnfo(0,10,"len=%d",len);
#endif
	if (buf && len) xnfo(0,10,"%s",buf);
	return 0;
}

int main(int argc, char* argv[])
{
	pthread_t dosbox;
	xnfo(0,1,"ALIVE!");

	if (XS_SDLInit()) xnfo(-1,1,"Unable to create SDL2 context!");
	xnfo(0,1,"SDL2 context created successfully");

	XS_ldb_register();

	if (pthread_create(&dosbox,NULL,Dosbox_Run,NULL))
		xnfo(-1,1,"Unable to create DOS thread!");
	xnfo(0,1,"DOSBox Thread running!");

	XS_SDLoop();

	if (pthread_join(dosbox,NULL)) xnfo(-1,1,"Threading error!");
	xnfo(0,1,"DOSBox Thread Exited");

	XS_SDLKill();
	xnfo(0,1,"QUIT");
	return (EXIT_SUCCESS);
}
