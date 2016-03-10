/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>


struct SSDHeader {
	uint32_t off,len;
};

int main(int argc, char* argv[])
{
	//init RNG
	srand(time(NULL));

	//chack args
	if (argc < 5) {
		printf("Usage: %s <card_size(GB)> <VM_RAM_size(MiB)> <program.bin> <output> [offset]\n",argv[0]);
		return 0;
	}

	//get SD size
	unsigned card = atoi(argv[1]);
	card *= 1000000000UL; //not binary, but decimal-based
	printf("Card size is %u bytes\n",card);

	//get VM RAM size
	unsigned vmram = atoi(argv[2]);
	vmram *= 1024 * 1024; //binary-based
	printf("VM RAM size is %u bytes\n",vmram);

	//open input file
	FILE* f = fopen(argv[3],"rb");
	if (!f) {
		printf("Unable to open file '%s'\n",argv[3]);
		return 1;
	}

	//get the file size
	fseek(f,0,SEEK_END);
	size_t sz = ftell(f);
	fseek(f,0,SEEK_SET);

	//and check it
	if (sz + 512 >= card) {
		printf("Program is too large for this card!\n");
		fclose(f);
		return 10;
	}

	printf("BIN file size = 0x%lX\n",sz);

	//allocate buffer
	char* bin = malloc(sz);
	if (!bin) abort();

	//and fill it
	int r = fread(bin,sz,1,f);
	fclose(f);

	if (r < 1) {
		printf("Unable to read all the bytes :(\n");
		return 2;
	}

	//now we can create output file
	f = fopen(argv[4],"wb");
	if (!f) {
		printf("Unable to create output file '%s'\n",argv[4]);
		return 3;
	}

	//generate offset or used provided one
	unsigned off = 512; //by default, the second sector
	if (argc == 6) {
		off = atoi(argv[5]);
		if (!off) {
			//zero offset means random offset generation
			off = ceil((float)rand() / (float)RAND_MAX * (float)(card - sz - 512*2 - 1)) + 512;
			//align it to nearest sector
			off /= 512;
			off *= 512;
		}
	}
	printf("RAM offset is %u, sector %u\n",off,off/512);

	//prepare data
	struct SSDHeader hd;
	hd.off = off;
	hd.len = vmram;
	char obuf[512]; //1 sector
	unsigned i,j;

	//start writing
	printf("Writing... ");
	for (i = 0, j = 0; (i < card) && (j < sz); i+=512) {
		memset(obuf,0,512);

		if (i == 0) {
			//write header
			memcpy(obuf,&hd,sizeof(hd));
		} else if (i >= off) {
			//write data
			if (j+512 >= sz)
				memcpy(obuf,bin+j,sz-j);
			else
				memcpy(obuf,bin+j,512);
			j += 512;
		} else {
			//write void :)
		}

		fwrite(obuf,512,1,f);
	}
	puts("done.");

	//ending
	fclose(f);
	free(bin);

	return 0;
}
