/*
** $ Herond Robaina Salles
** Prefix: lst
*/

/* ========================================================================== */

#ifndef _LST_LIB
#define _LST_LIB

#include <errno.h>

/* ========================================================================== */

#define LST_DEFAULT_COMPARATOR (lstDefaultComparator)

/* ========================================================================== */

enum LSTErros_EN {
    LST_NO_ERROR = 0,
    LST_NOT_ENOUGH_SPACE = ENOMEM,
    LST_INVALID_ARGUMENT = EINVAL
};

struct LSTList_ST;

typedef struct LSTList_ST* LSTList;

/* ========================================================================== */

LSTList lstNew (void);

LSTList lstCopy (const LSTList self);

void lstClear (LSTList self);

void lstDestroy (LSTList* self_p);

void lstAppend (LSTList self, void* elem_p);

void lstInject (LSTList list, void* elem_p);

void lstInsert (LSTList self, unsigned pos, void* elem_p);

void lstSetElement (LSTList self, unsigned pos, void* elem_p);

void* lstLookElement (const LSTList self, unsigned pos);

void* lstLookLastElement (const LSTList self);

void* lstLookFirstElement (const LSTList self);

void* lstGetElement (LSTList self, unsigned pos);

void* lstGetLastElement (LSTList self);

void* lstGetFirstElement (LSTList self);

void lstReverse (LSTList self);

unsigned lstGetLength (const LSTList self);

LSTList lstSearch (
    const LSTList self,
    int (*comparator) (const void*, void*),
    void* elem_comp_p
);

void* lstSearchElement (
    const LSTList self,
    int (*comparator) (const void*, void*),
    void* elem_comp_p
);

int lstGetError (void);

void lstPrint (const LSTList list);

/* ========================================================================== */

int lstDefaultComparator (const void* elem_p, void* elem_comp_p);

/* ========================================================================== */

#endif

