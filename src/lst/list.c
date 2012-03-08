/*
** $ Herond Robaina Salles
** Prefix: plt
*/

/* ========================================================================== */

#ifndef _LST_IMP
#define _LST_IMP

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/* ========================================================================== */

int lst_current_error = LST_NO_ERROR;

/* ========================================================================== */

struct LSTNode_ST {
    void* elem_p;
    struct LSTNode_ST* next;
    struct LSTNode_ST* previous;
};

struct LSTList_ST {
    unsigned size;
    struct LSTNode_ST* first;
    struct LSTNode_ST* last;
};

/* ========================================================================== */

static struct LSTNode_ST* lstGetNode (LSTList self, unsigned pos) {
    struct LSTNode_ST* node_p = NULL;

    if (self && (pos < self->size)) {
        int i;

        for (
            i = 0, node_p = self->first;
            (i < pos) || (node_p == NULL);
            i ++, node_p = node_p->next
        );
    }

    return node_p;
}

/* ========================================================================== */

LSTList lstNew (void) {
    LSTList list_return = (LSTList) malloc(sizeof(struct LSTList_ST));

    if (list_return) {
        list_return->size = 0;
        list_return->first = NULL;
        list_return->last = NULL;
    } else lst_current_error = LST_NOT_ENOUGH_SPACE;

    return list_return;
}

LSTList lstCopy (const LSTList self) {
    if (self) {
        int i;
        LSTList list_return = lstNew();

        if (list_return) {
            for (i = 0; i < self->size; i ++)
                lstAppend(list_return, lstLookElement(self, i));

            return list_return;
        }

        return NULL;
    } lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void lstClear (LSTList self) {
    if (self) while (self->size) lstGetFirstElement(self);
    else lst_current_error = LST_INVALID_ARGUMENT;
}

void lstDestroy (LSTList* self_p) {
    if (self_p) {
        lstClear(*self_p);
        free(*self_p);
        *self_p = NULL;
    } else lst_current_error = LST_INVALID_ARGUMENT;
}

void lstAppend (LSTList self, void* elem_p) {
    if (self && elem_p) {
        struct LSTNode_ST* node_p = (struct LSTNode_ST*) malloc(
            sizeof(struct LSTNode_ST)
        );

        if (node_p) {
            node_p->next = NULL;
            node_p->previous = self->last;
            node_p->elem_p = elem_p;

            if (self->last) self->last->next = node_p;
            self->last = node_p;
            if (! self->size) self->first = node_p;
            self->size ++;
        } else lst_current_error = LST_NOT_ENOUGH_SPACE;
    } else lst_current_error = LST_INVALID_ARGUMENT;
}

void lstInject (LSTList self, void *elem_p) {
    if (self && elem_p) {
        struct LSTNode_ST* node_p = (struct LSTNode_ST*) malloc(
            sizeof(struct LSTNode_ST*)
        );

        if (node_p) {
            node_p->next = self->first;
            node_p->previous = NULL;
            node_p->elem_p = elem_p;

            if (self->first) self->first->previous = node_p;
            self->first = node_p;
            self->size ++;
        } else lst_current_error = LST_NOT_ENOUGH_SPACE;
    } else lst_current_error = LST_INVALID_ARGUMENT;
}

void lstInsert (LSTList self, unsigned pos, void* elem_p) {
    struct LSTNode_ST* node_p = NULL;

    if (self && elem_p && (pos < self->size)) {
        if (pos == self->size - 1) lstAppend(self, elem_p);
        else if (pos == 0) lstInject(self, elem_p);
        else {
            node_p = (struct LSTNode_ST*) malloc(sizeof(struct LSTNode_ST));
            struct LSTNode_ST* curr_node_p = lstGetNode(self, pos);

            if (node_p) {
                node_p->next = curr_node_p;
                node_p->previous = curr_node_p->previous;
                node_p->elem_p = elem_p;

                curr_node_p->previous->next = node_p;
                curr_node_p->previous = node_p;

                self->size ++;
            } else lst_current_error = LST_NOT_ENOUGH_SPACE;
        }
    } else lst_current_error = LST_INVALID_ARGUMENT;
}

void lstSetElement (LSTList self, unsigned pos, void* elem_p) {
    struct LSTNode_ST* node_p = lstGetNode(self, pos);

    if (node_p) node_p->elem_p = elem_p;
}

void* lstLookElement (const LSTList self, unsigned pos) {
    if (self && (pos < self->size))
        return lstGetNode(self, pos)->elem_p;
    else lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void* lstLookLastElement (const LSTList self) {
    if (self && self->size) return self->last->elem_p;
    lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void* lstLookFirstElement (const LSTList self) {
    if (self && self->size) return self->first->elem_p;
    lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void* lstGetElement (LSTList self, unsigned pos) {
    if (self && self->size) {
        if (! pos) return lstGetFirstElement(self);
        else if (pos == self->size - 1) return lstGetLastElement(self);
        else {
            struct LSTNode_ST* node_p = lstGetNode(self, pos);
            void* elem_p = node_p->elem_p;

            node_p->previous->next = node_p->next;
            node_p->next->previous = node_p->previous;
            self->size --;
            free(node_p);

            return elem_p;
        }
    }

    lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void* lstGetLastElement (LSTList self) {
    if (self && self->size) {
        struct LSTNode_ST* last_node_p = self->last;
        void* elem_p = self->last->elem_p;

        if (self->last->previous) self->last->previous->next = NULL;
        else self->first = NULL;
        self->last = self->last->previous;
        self->size --;
        free(last_node_p);

        return elem_p;
    }

    lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void* lstGetFirstElement (LSTList self) {
    if (self && self->size) {
        struct LSTNode_ST* first_node_p = self->first;
        void* elem_p = self->first->elem_p;

        if (self->first->next) self->first->next->previous = NULL;
        else self->last = NULL;
        self->first = self->first->next;
        self->size --;
        free(first_node_p);

        return elem_p;
    }

    lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void lstReverse (LSTList self) {
    int i;
    void* elem_p = NULL;

    for (i = 0; i < self->size / 2; i ++) {
        elem_p = lstLookElement(self, i);
        lstSetElement(self, i, lstLookElement(self, self->size - 1 - i));
        lstSetElement(self, self->size - 1 - i, elem_p);
    }
}

unsigned lstGetLength (const LSTList self) {
    if (self) return self->size;
    else lst_current_error = LST_INVALID_ARGUMENT;

    return 0;
}

LSTList lstSearch (
    const LSTList self,
    int (*comparator) (const void*, void*),
    void* elem_comp_p
) {
    if (self && elem_comp_p) {
        int i;
        struct LSTNode_ST* node_p = NULL;
        LSTList list_return = lstNew();

        comparator = comparator ? comparator : LST_DEFAULT_COMPARATOR;

        if (list_return) {
            for (
                i = 0, node_p = self->first;
                i < self->size;
                i ++, node_p = node_p->next
            ) if (comparator(node_p->elem_p, elem_comp_p))
                lstAppend(list_return, node_p->elem_p);

            return list_return;
        }
    } else lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

void* lstSearchElement (
    const LSTList self,
    int (*comparator) (const void*, void*),
    void* elem_comp_p
) {
    if (self && elem_comp_p) {
        int i;
        struct LSTNode_ST* node_p = NULL;

        comparator = comparator ? comparator : LST_DEFAULT_COMPARATOR;

        for (
            i = 0, node_p = self->first;
            i < self->size;
            i ++, node_p = node_p->next
        ) if (comparator(node_p->elem_p, elem_comp_p))
            return node_p->elem_p;

    } else lst_current_error = LST_INVALID_ARGUMENT;

    return NULL;
}

int lstGetError (void) {
    int error_return = lst_current_error;

    lst_current_error = LST_NO_ERROR;

    return error_return;
}

void lstPrint (const LSTList self) {
    if (self) {
        int i = 0;
        struct LSTNode_ST* node_p = NULL;

        for (node_p = self->first; i < self->size; node_p = node_p->next, i ++)
            printf("%i:%p ", i, node_p->elem_p);

        puts("");
    } else printf("(nil)\n");
}

/* ========================================================================== */

int lstDefaultComparator (const void* elem_p, void* elem_comp_p) {
    return elem_p == elem_comp_p;
}

/* ========================================================================== */

#endif

