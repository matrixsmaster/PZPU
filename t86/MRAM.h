// (C) Dmitry 'MatrixS_Master' Soloviov, 2015

#ifndef MRAM_H_
#define MRAM_H_

#include "VM86conf.h"

#define MRAMQUEUE 65536

typedef unsigned char uch;

/* Testing managed RAM unit */
/*
class RAM
{
private:
	uch mem[RAM_SIZE];

protected:
	uch que[MRAMQUEUE];
	int last;
	uch scr,old;
	bool wr,wq;

	void Test() {
		if (wr) {
			wr = false;
			if (old != scr)
				mem[last] = scr;
		}
		if (wq) {
			wq = false;
			memcpy(mem+last,que,sizeof(que));
		}
	}

public:
	RAM() {
		memset(mem,0,sizeof(mem));
		memset(que,0,sizeof(que));
		last = 0;
		scr = 0;
		old = 0;
		wr = wq = false;
	}

	int Max() const { return RAM_SIZE; }

	uch & operator [] (const int a) {
		Test();
		if ((a >= 0) && (a < RAM_SIZE)) {
			last = a;
			old = scr = mem[a];
			wr = true;
		} else {
			last = 0;
			old = scr = 0;
		}
		return scr;
	}

	uch * operator + (const int s) {
		Test();
		if ((s < 0) || (s >= RAM_SIZE))
			return NULL;
		last = s;
		wq = true;
		memcpy(que,mem+last,sizeof(que));
		return que;
	}
};
*/
#include <iostream>
#define MRAMDEPTH 16
using namespace std;
class RAM
{
private:
	uch mem[RAM_SIZE];

protected:
	uch que[MRAMDEPTH][MRAMQUEUE];
	int last;
	int ql[MRAMDEPTH];
	uch scr,old;
	bool wr,wq;

	void Test() {
		if (wr) {
			wr = false;
			if (old != scr)
				mem[last] = scr;
		}
		if (wq) {
			cout << "Copying" << endl;
			wq = false;
			for (int i = 0; i < MRAMDEPTH; i++)
				if (ql[i] >= 0) {
					memcpy(mem+ql[i],&(que[i][0]),MRAMQUEUE);
					//ql[i] = -1;
				}
		}
	}

public:
	RAM() {
		memset(mem,0,sizeof(mem));
		memset(que,0,sizeof(que));
		last = 0;
		scr = 0;
		old = 0;
		wr = wq = false;
		for (int i = 0; i < MRAMDEPTH; i++)
			ql[i] = -1;
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

		if (wq)
			cout << "Copied" << endl;
		else {
			cout << "No room left for operation" << endl;
			return NULL;
		}

		return &(que[i][0]);
	}
};
#endif /* MRAM_H_ */
