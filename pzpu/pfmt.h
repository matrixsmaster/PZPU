/* PZPU - Pseudo-ZPU emulator
 * (C) MatrixS_Master, 2016
 * GPL v2
 */

#ifndef PFMT_H_
#define PFMT_H_

#if EMBED_AVR

#define PFMT_32XINT "%08lX"
#define PFMT_32UINT "%lu"

#else /* PC */

#define PFMT_32XINT "%08X"
#define PFMT_32UINT "%u"

#endif

#endif /* PFMT_H_ */
