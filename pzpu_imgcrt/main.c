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
static unsigned blkoff = 0; //no blockwise offset
static int fill = -1; //don't fill by default
static int mode = 0; //normal mode
static const char* fn_in = NULL; //input file
static const char* fn_out = NULL; //output file

//Helper functions
static void printsettings()
{
	if (fn_out) printf("Image file:\t%s\n",fn_out);
	if (fn_in) printf("Binary file:\t%s\n",fn_in);
	printf("Card size:\t%u GB\n",card);
	printf("RAM size:\t%u MiB\n",vmram);
	printf("RAM offset:\t%u bytes (%u sectors)\n",off,off/512);
	printf("Blockwise off.:\t%u bytes\n",blkoff);

	if (fill >= 0)
		printf("Filler byte:\t0x%02X\n",fill);

	printf("Image mode: ");
	switch (mode) {
	case 0: puts("Normal"); break;
	case 1: puts("Dotted"); break;
	default: puts("Invalid"); break;
	}
}

static void help(const char* prog)
{
	int i;
	printf("Usage: %s [<options>], which are:\n",prog);
	for (i = 0; arg_table[i].desc; i++)
		printf("\t-%c: %s\n",arg_table[i].sw,arg_table[i].desc);
	printf("\nDefault settings are:\n");
	printsettings();
}

static int argparse(int argc, char* argv[])
{
	int i,j;
	enum EICArgument p = ICA_INVALID;

	for (i = 1; i < argc; i++) {
		switch (p) {
		case ICA_INVALID:
			//awaiting for next switch, check it
			if ((strlen(argv[i]) != 2) || (argv[i][0] != '-')) {
				printf("Invalid argument %d ('%s').\n",i,argv[i]);
				return 0;
			}
			//try to resolve switch char
			for (j = 0; arg_table[j].typ != ICA_INVALID; j++)
				if (arg_table[j].sw == argv[i][1])
					p = arg_table[j].typ;
			//error message
			if (p == ICA_INVALID) {
				printf("Unknown switch '%c' at argument %d.\n",argv[i][1],i);
				return 0;
			}
			break;

		case ICA_MODE:
			mode = atoi(argv[i]);
			if ((mode < 0) || (mode > 1)) {
				printf("Invalid mode setting.\n");
				return 0;
			}
			p = ICA_INVALID;
			break;

		case ICA_IMGFILE:
			fn_out = argv[i];
			p = ICA_INVALID;
			break;

		case ICA_BINFILE:
			fn_in = argv[i];
			p = ICA_INVALID;
			break;

		case ICA_CARDSZ:
			card = atoi(argv[i]);
			if ((!card) || (card > 999)) {
				printf("Invalid card size value.\n");
				return 0;
			}
			p = ICA_INVALID;
			break;

		case ICA_RAMSZ:
			vmram = atoi(argv[i]);
			if (!vmram) {
				printf("Invalid RAM size value.\n");
				return 0;
			}
			p = ICA_INVALID;
			break;

		case ICA_OFFSET:
			off = atoi(argv[i]);
			if (!off) {
				//zero offset means random offset generation
				off = ceil((float)rand() / (float)RAND_MAX * (float)(card - vmram - 512*2 - 1)) + 512;
				//align it to nearest sector
				off /= 512;
				off *= 512;
			}
			p = ICA_INVALID;
			break;

		case ICA_BLKOFF:
			blkoff = atoi(argv[i]);
			if (!off) {
				//zero offset means random offset generation
				blkoff = ceil((float)rand() / (float)RAND_MAX * (float)(512 - 5));
				//align it to nearest sector
				blkoff /= 512;
				blkoff *= 512;
			}
			p = ICA_INVALID;
			break;

		case ICA_FILLER:
			fill = atoi(argv[i]) & 255;
			p = ICA_INVALID;
			break;
		}
	}

	return 1;
}

//The Image creator itself
static void create_normal(FILE* f, const char* bin, size_t len)
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
				abort();
			}
			//write something void :) specifically - zeroes
		}

		fwrite(obuf,512,1,f);
	}
	printf("done.\n%u sectors wrote.\n",n);
}

//Term "dotted" selected to not to confuse with more applicable, but reserved term "sparsed".
//The true "sparsed" image will use POSIX OS' filesystems capability of using "holes" in large
//files to preserve space. And I want to use it later :)
static void create_dotted(FILE* f, const char* bin, size_t len)
{
	//TODO
}

//The Entry Point
int main(int argc, char* argv[])
{
	//init RNG
	srand(time(NULL));

	//print banner
	printf("PZPU SD Image Creator. (C) MatrixS_Master, 2016\n\n");

	//read arguments (min. 2 (in/out))
	if ((argc < 5) || (!argparse(argc,argv)) || (!fn_in) || (!fn_out)) {
		help(argv[0]);
		return 0;
	}
	printsettings();

	//calculate SD size
	card *= 1000000000UL; //not binary, but decimal-based (10^9)
	printf("Card size is 0x%X (%u) bytes\n",card,card);

	//calculate VM RAM size
	vmram *= 1024 * 1024; //binary-based (2^20)
	printf("VM RAM size is 0x%X (%u) bytes\n",vmram,vmram);

	//check offset
	if ((mode == 0) && (off < 512)) {
		puts("ERROR: The sector Zero is reserved. Image offset can't be less than 512 bytes.");
		return 11;
	}
	if (off + vmram + sizeof(struct SSDHeader) >= card) {
		puts("ERROR: Unable to fit into the card.");
		if (vmram + sizeof(struct SSDHeader) >= card)
			puts("Try to use lower offset value.");
		return 11;
	}

	//check space requirements for dotted image
	if (mode == 1) {
		if (card / 128 <= vmram + sizeof(struct SSDHeader)) {
			puts("ERROR: Unable to fit into the card.");
			puts("You must take into consideration, that dotted RAM should be at least 128 times smaller than SD card size.");
			return 12;
		}
		if (card / 128 <= vmram + sizeof(struct SSDHeader) + off) {
			puts("ERROR: Unable to fit into the card. Offset value is too high. Try to use zero offset.");
			return 12;
		}
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
