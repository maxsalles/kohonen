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

typedef double** Connections;

struct KHNNet_ST {
    unsigned width, height, input_len;
    Connections* grid;
};

/* ========================================================================== */

Connections newConnections (unsigned width, unsigned height) {
    int i;
    Connections connections_return = (Connections)
        malloc(sizeof(double*) * height);

    for (i = 0; i < height; i ++)
        connections_return[i] = (double*) malloc(sizeof(double) * width);

    return connections_return;
}

void destroyConnections (Connections connections, unsigned height) {
    int i;

    for (i = 0; i < height; i ++) free(connections[i]);

    free(connections);
}

Connections copyConnections (
    Connections connections, unsigned width, unsigned height
) {
    int i, j;
    Connections copy = newConnections(width, height);

    for (i = 0; i < height; i ++)
        for (j = 0; j < width; j ++)
            copy[i][j] = connections[i][j];

    return copy;
}

/* ========================================================================== */

KHNNet khnNew (unsigned width, unsigned height, unsigned input_len) {
    int i;
    KHNNet net_return = (KHNNet) malloc(sizeof(struct KHNNet_ST));

    net_return->width = width;
    net_return->height = height;
    net_return->input_len = input_len;
    net_return->grid = (Connections*) malloc(sizeof(Connections) * input_len);

    for (i = 0; i < input_len; i ++)
        net_return->grid[i] = newConnections(width, height);

    khnClear(net_return);

    return net_return;
}

void khnDestroy (KHNNet* self_p) {
    int i;

    for (i = 0; i < (*self_p)->input_len; i ++)
        destroyConnections((*self_p)->grid[i], (*self_p)->height);

    free((*self_p)->grid);
    free(*self_p);
    *self_p = NULL;
}

KHNNet khnCopy (KHNNet self) {
    int i;
    KHNNet copy = khnNew(self->width, self->height, self->input_len);

    for (i = 0; i < self->input_len; i ++)
        copy->grid[i] = copyConnections(
            self->grid[i], self->width, self->height
        );

    return copy;
}

void khnClear (KHNNet self) {
    int i, j, k;

    srand(time(NULL));

    for (i = 0; i < self->input_len; i ++)
        for (j = 0; j < self->height; j ++)
            for (k = 0; k < self->width; k ++)
                self->grid[i][j][k] = (double) rand() / (double) RAND_MAX;
}

void khnPrint (const KHNNet self) {
    int i, j, k;

    for (i = 0; i < self->input_len; i ++) {
        printf("%i:\n", i);
        for (j = 0; j < self->height; j ++) {
            for (k = 0; k < self->width; k ++)
                printf("%f ", self->grid[i][j][k]);

            puts("");
        }
    }
}

/* ========================================================================== */


#endif

