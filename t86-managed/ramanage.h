//(C) Dmitry 'MatrixS_Master' Soloviov, 2015

#ifndef RAMANAGE_H_
#define RAMANAGE_H_

typedef unsigned char uch;

void ram_init();
uch ramrd(int adr);
uch ramwr(int adr, uch x);

#endif /* RAMANAGE_H_ */
