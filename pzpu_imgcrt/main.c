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
#include "pzpuimg.h"

//Global variables and its default settings
static unsigned card = 4; //4 GB TF card
static unsigned vmram = 2; //2 MiB virtual RAM
static unsigned off = 512; //by default, the second sector
static int fill = -1; //don't fill by default
static int mode = 0; //normal mode
static const char* fn_in = NULL; //input file
static const char* fn_out = NULL; //output file

//Helper functions
void help(const char* prog)
{
	printf("Usage: %s <card_size(GB)> <VM_RAM_size(MiB)> <program.bin> <output> [offset] [fill_byte]\n",prog);
}

int argparse(int argc, char* argv[])
{
	return 0;
	/*if (argc >= 6) {
		off = atoi(argv[5]);
		if (!off) {
			//zero offset means random offset generation
			off = ceil((float)rand() / (float)RAND_MAX * (float)(card - vmram - 512*2 - 1)) + 512;
			//align it to nearest sector
			off /= 512;
			off *= 512;
		}
	}*/
	//if (argc >= 7) fill = atoi(argv[6]);
}

//The Image creator itself
void create_normal(FILE* f, const char* bin, size_t len)
{
	struct SSDHeader hd;
	hd.off = off;
	hd.len = vmram;
	char obuf[512]; //1 sector
	unsigned i,j,n;

	//start writing
	printf("Writing... ");
	fflush(stdout);
	for (i = 0, j = 0, n = 0; i < card; i += 512, n++) {
		memset(obuf,0,512);
		//FIXME: unaligned access isn't possible so far!

		if (i == 0) {
			//write header
			memcpy(obuf,&hd,sizeof(hd));

		} else if (i >= off) {
			if (j < len) {
				//write data
				if (j+512 >= len)
					memcpy(obuf,bin+j,len-j);
				else
					memcpy(obuf,bin+j,512);

			} else if ((fill >= 0) && (j < vmram)) {
				//fill virtual memory area
				memset(obuf,(char)fill,512);

			} else {
				//nothing to write anymore
				break;

			}
			j += 512;

		} else /* between header and data area */ {
			if ((i < off) && (i + 512 > off)) {
				//the next sector will contain only a part of data (unaligned access)
				//TODO
			}
			//write something void :) specifically - zeroes
		}

		fwrite(obuf,512,1,f);
	}
	printf("done.\n%u sectors wrote.\n",n);
}

//Term "dotted" selected to not to confuse with more applicable, but reserved term "sparsed".
//The true "sparsed" image will use POSIX OS' filesystems capability of using "holes" in large
//files to preserve space.
void create_dotted(FILE* f, const char* bin, size_t len)
{
	//TODO
}

//The Entry Point
int main(int argc, char* argv[])
{
	//init RNG
	srand(time(NULL));

	//read arguments
	if (!argparse(argc,argv)) {
		help(argv[0]);
		return 0;
	}

	//calculate SD size
	card *= 1000000000UL; //not binary, but decimal-based (10^9)
	printf("Card size is %u bytes\n",card);

	//calculate VM RAM size
	vmram *= 1024 * 1024; //binary-based (2^20)
	printf("VM RAM size is 0x%X (%u) bytes\n",vmram,vmram);

	//check offset
	printf("RAM offset is %u, sector %u\n",off,off/512);
	if (off < 512) {
		printf("ERROR: The sector Zero is reserved. Image offset can't be less than 512 bytes.\n");
		return 11;
	}

	//open input file
	FILE* f = fopen(fn_in,"rb");
	if (!f) {
		printf("ERROR: Unable to open file '%s'\n",fn_in);
		return 1;
	}

	//get the file size
	fseek(f,0,SEEK_END);
	size_t sz = ftell(f);
	fseek(f,0,SEEK_SET);

	//and check it
	if (vmram + 512 >= card) {
		printf("ERROR: Project is too large for this card!\n");
		fclose(f);
		return 10;
	}
	printf("BIN file size = 0x%lX (%lu) bytes\n",sz,sz);

	//allocate buffer
	char* bin = malloc(sz);
	if (!bin) abort();

	//and fill it
	int r = fread(bin,sz,1,f);
	fclose(f);

	if (r < 1) {
		printf("ERROR: Unable to read all the bytes :(\n");
		return 2;
	}

	//now we can create output file
	f = fopen(fn_out,"wb");
	if (!f) {
		printf("ERROR: Unable to create output file '%s'\n",fn_out);
		return 3;
	}

	//and do the work
	switch (mode) {
	case 0:
		printf("Creating normal (flat) image...\n");
		create_normal(f,bin,sz);
		break;

	case 1:
		printf("Creating dotted image...\n");
		create_dotted(f,bin,sz);
		break;

	default:
		printf("ERROR: Unknown mode of operation!\n");
		break;
	}

	//ending
	fclose(f);
	free(bin);

	return 0;
}
