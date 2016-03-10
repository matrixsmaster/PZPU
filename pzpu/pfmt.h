/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef PFMT_H_
#define PFMT_H_

#if EMBED_AVR

#define PFMT_32XINT "%08lX"

#define PFMT_16UINT "%u"
#define PFMT_32UINT "%lu"
#define PFMT_64UINT "%llu"

#else /* PC */

#define PFMT_32XINT "%08X"

#define PFMT_16UINT "%hu"
#define PFMT_32UINT "%u"
#define PFMT_64UINT "%lu"

#endif

#endif /* PFMT_H_ */
