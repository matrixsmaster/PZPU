// (C) Dmitry 'MatrixS_Master' Soloviov, 2015

#ifndef MRAM_H_
#define MRAM_H_

#include "VM86conf.h"
#include <iostream>

#define MRAMDEPTH 0
#define MRAMQUEUE 768

typedef unsigned char uch;

using namespace std;

/* Testing managed RAM unit (quick and dirty) */

class RAM
{
private:
	uch mem[RAM_SIZE];

protected:
	int last;
	uch scr,old;
	bool wr;

#if MRAMDEPTH > 0
	bool wq;
	uch que[MRAMDEPTH][MRAMQUEUE];
	int ql[MRAMDEPTH];
#endif

	void Test() {
		if (wr) {
			wr = false;
			if (old != scr)
				mem[last] = scr;
		}
#if MRAMDEPTH > 0
		if (wq) {
			wq = false;
			for (int i = 0; i < MRAMDEPTH; i++)
				if (ql[i] >= 0) {
					memcpy(mem+ql[i],&(que[i][0]),MRAMQUEUE);
					//ql[i] = -1;
				}
		}
#endif
	}

public:
	RAM() {
		memset(mem,0,sizeof(mem));
		last = 0;
		scr = 0;
		old = 0;
		wr = false;

#if MRAMDEPTH > 0
		memset(que,0,sizeof(que));
		wq = false;
		for (int i = 0; i < MRAMDEPTH; i++)
			ql[i] = -1;
#endif

	}

	int Max() const { return RAM_SIZE; }

	uch & operator [] (const int a) {
		Test();
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

#if MRAMDEPTH > 0
	uch * operator + (const int s) {
		int i;

		Test();
		cout << "Access to RAM w/offset " << s << endl;
		if ((s < 0) || (s >= RAM_SIZE))
			return NULL;

		for (i = 0; i < MRAMDEPTH; i++) {
			if (ql[i] == s) {
				wq = true;
				break;
			} else if (ql[i] == -1) {
				memcpy(&(que[i][0]),mem+s,MRAMQUEUE);
				wq = true;
				ql[i] = s;
				break;
			}
		}

		if (!wq) {
			cout << "No room left for operation" << endl;
			return NULL;
		}

		return &(que[i][0]);
	}
#endif
};

/*
class RAMptr
{
private:
	RAM* ram;
	int off;

public:
//	RAMptr() { ram = NULL; off = 0; }
//	RAMptr(RAM* p) { ram = p; off = 0; }
	RAMptr(RAM* p = NULL, int o = 0) { ram = p; off = o; }

	RAMptr operator + (const int s) { return RAMptr(ram,off+s); }
	RAMptr & operator += (const int s) { off += s; return &this; }
};
*/

#endif /* MRAM_H_ */
