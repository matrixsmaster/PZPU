// (C) Dmitry 'MatrixS_Master' Soloviov, 2015

#ifndef MRAM_H_
#define MRAM_H_

#include "VM86conf.h"

typedef unsigned char uch;

/* Testing managed RAM unit (quick and dirty) */

class RAM
{
private:
	uch mem[RAM_SIZE];
	int last;
	uch scr,old;
	bool wr;

public:
	RAM() {
		memset(mem,0,sizeof(mem));
		last = 0;
		scr = 0;
		old = 0;
		wr = false;
//		cout << "RAM init OK" << endl;
	}

	int Max() const { return RAM_SIZE; }

	uch & operator [] (const int a) {
//		cout << "[]" << endl;
		if (wr) {
//			cout << "Flag" << endl;
			wr = false;
			if (old != scr) {
				mem[last] = scr;
//				cout << "Wrote" << endl;
			}
		}
		if ((a >= 0) && (a < RAM_SIZE)) {
			last = a;
			old = scr = mem[a];
			wr = true;
			return scr;
		} else {
			last = 0;
			old = scr = 0;
			return scr;
		}
	}
};

class RAMptr
{
private:
	RAM* ram;
	int off;

public:
	RAMptr(RAM* p = NULL, int o = 0) { ram = p; off = o; }

	RAMptr operator + (const int s) { return RAMptr(ram,off+s); }
	RAMptr & operator += (const int s) { off += s; return *this; }
	uch & operator [] (const int s) { return (*ram)[off+s]; }
	uch & operator * (void) { return (*ram)[off]; }
};

#endif /* MRAM_H_ */
