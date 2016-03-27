/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef PZPUIMG_H_
#define PZPUIMG_H_

enum EICArgument {
	ICA_BINFILE,
	ICA_IMGFILE,
	ICA_CARDSZ,
	ICA_RAMSZ,
	ICA_MODE,
	ICA_OFFSET,
	ICA_BLKOFF,
	ICA_FILLER,
	ICA_INVALID
};

struct SICArgument {
	enum EICArgument typ;
	char sw;
	const char* desc;
};

struct SSDHeader {
	uint32_t off,len;
};

static const struct SICArgument arg_table[] = {
		{ ICA_BINFILE,	'i', "Input binary file with compiled program." },
		{ ICA_IMGFILE,	'o', "Output file with generated SD card image." },
		{ ICA_CARDSZ,	's', "Size of target SD card, in GB." },
		{ ICA_RAMSZ,	'm', "Size of virtual RAM area for target system, in MiB." },
		{ ICA_OFFSET,	'f', "Image starting point offset, in bytes." },
		{ ICA_BLKOFF,	'b', "Blockwise offset for dotted images, in bytes." },
		{ ICA_FILLER,	'r', "Fill unused bytes in virtual RAM with some value." },
		{ ICA_MODE,		'd', "Image mode: 0 - normal image, 1 - dotted." },
		{ ICA_INVALID,	0, NULL }
};

#endif /* PZPUIMG_H_ */
