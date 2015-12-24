//(C) Dmitry 'MatrixS_Master' Soloviov, 2015

#include <stdio.h> //FIXME
#include "ramanage.h"

void ram_init()
{
	printf("DEBUG: RAM init fired\n");
}

uch ramrd(int adr)
{
	printf("DEBUG: RAM read %d\n",adr);
	//TODO
	return 0;
}

uch ramwr(int adr, uch x)
{
	printf("DEBUG: RAM write %d : %hhu\n",adr,x);
	//TODO
	return x;
}
