/*
** $ Herond Robaina Salles, 2012
** Prefix: khn
*/

#ifndef _KHN_IMP
#define _KHN_IMP

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "kohonen.h"

/* ========================================================================== */

struct KHNNet_ST {
    unsigned width, height;
    double** grid;
};

/* ========================================================================== */

KHNNet khnNew (unsigned width, unsigned height) {
    int i;
    KHNNet net_return = (KHNNet) malloc(sizeof(struct KHNNet_ST));

    net_return->width = width;
    net_return->height = height;
    net_return->grid = (double**) malloc(sizeof(double*) * height);

    for (i = 0; i < height; i ++)
        net_return->grid[i] = (double*) malloc(sizeof(double) * width);

    khnClear(net_return);

    return net_return;
}

void khnDestroy (KHNNet* self_p) {
    int i;

    for (i = 0; i < (*self_p)->height; i ++) free((*self_p)->grid[i]);

    free((*self_p)->grid);
    free(*self_p);
    *self_p = NULL;
}

KHNNet khnCopy (KHNNet self) {
    int i, j;
    KHNNet net_copy = khnNew(self->width, self->height);

    for (i = 0; i < self->height; i ++)
        for (j = 0; j < self->width; j ++)
            net_copy->grid[i][j] = self->grid[i][j];

    return net_copy;
}

void khnClear (KHNNet self) {
    int i, j;

    srand(time(NULL));

    for (i = 0; i < self->height; i ++)
        for (j = 0; j < self->width; j ++)
            self->grid[i][j] = (float) rand() / (float) RAND_MAX;
}

void khnPrint (const KHNNet self) {
    int i, j;

    for (i = 0; i < self->height; i ++) {
        for (j = 0; j < self->width; j ++) printf("%f ", self->grid[i][j]);
        puts("");
    }
}

/* ========================================================================== */


#endif

