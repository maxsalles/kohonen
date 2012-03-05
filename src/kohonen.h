/*
** $ Herond Robaina Salles, 2012
** Prefix: khn
*/

#ifndef _KHN_LIB
#define _KHN_LIB

/* ========================================================================== */

struct KHNNet_ST;

typedef struct KHNNet_ST* KHNNet;

/* ========================================================================== */

KHNNet khnNew (unsigned width, unsigned height);

void khnDestroy (KHNNet* self_p);

KHNNet khnCopy (KHNNet self);

void khnClear (KHNNet self);

void khnPrint (const KHNNet self);

/* ========================================================================== */

#endif

