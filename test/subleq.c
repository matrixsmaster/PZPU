// Subleq experiment
//(C) Dmitry 'MatrixS_Master' Solovyev, 2019

//#define NDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#define RAMSIZE (1024*1024*32/4)
#define SCANPAT "%ld"
typedef int64_t bint;

static bint ram[RAMSIZE];

int main(int argc, char* argv[])
{
    printf("Subleq interpreter by MatrixS_Master.\nBase type size = %lu\n\n",sizeof(bint));
    assert(argc >= 2);

    int step = ((argc > 2) && (!strcmp(argv[2],"-s")))? 1 : 0;

    FILE* fi = fopen(argv[1],"r");
    assert(fi);

    bint val, ip = 0, maxip = 0;
    while (fscanf(fi,SCANPAT,&val) > 0) {
        ram[ip++] = val;
        assert(ip < RAMSIZE);
    }
    fclose(fi);
    printf("Last val = " SCANPAT ", ip = " SCANPAT "\n", val,ip);

    ip = 0;
    bint a,b,c;
    size_t n = 0;
    do {
        a = ram[ip++];
        b = ram[ip++];
        c = ram[ip++];
        assert(ip < RAMSIZE);

        if (step) {
            printf("N = %lu, IP = %ld, Ra = %ld, Rb = %ld, Rc = %ld, RRa = %ld, RRb = %ld\n",n,ip,a,b,c,ram[a],ram[b]);
            getchar();
        }

        assert((a == -1 && b >= 0) || (b == -1 && a >= 0) || (a >= 0 && b >= 0));

        if (a == -1) {
            ram[b] += getchar();
            if (ram[b] <= 0) ip = c;

        } else if (b == -1) {
            putchar(ram[a]);
            fflush(stdout);

        } else {
            ram[b] -= ram[a];
            if(ram[b] <= 0) ip = c;

        }

        if (ip > maxip) maxip = ip;
        n++;
    } while (ip >= 0);

    printf("\nExecution complete. Top IP = " SCANPAT "\nDone.\n",maxip);
    return 0;
}
