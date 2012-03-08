/*
** $ Herond Robaina Salles
** Prefix: khn
*/

#ifndef _KHN_IMP
#define _KHN_IMP

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

#include "kohonen.h"

/* ========================================================================== */

typedef double** Connections;

struct KHNTraining_ST {
    KHNNet net;
    unsigned cur_iter;
    double neighbourhood, larning_rate, time_const;
};


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

static double averageDistance (
    KHNNet self, unsigned x, unsigned y, const double input[]
) {
    int i;
    double distance = 0.0;

    for (i = 0; i < self->input_len; i ++)
        distance += pow(input[i] - self->grid[i][x][y], 2.0);

    return distance;
}

static double distance (
    unsigned x_i, unsigned y_i,
    unsigned x_j, unsigned y_j
) {
    return sqrt(pow((float) x_i - x_j, 2.0) + pow((float) y_i - y_j, 2.0));
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

KHNResult_ST khnGetResult (KHNNet self, const double input[]) {
    int i, j;
    KHNResult_ST result;
    double cur_distance = 0.0, min_distance = (double) FLT_MAX;

    for (i = 0; i < self->height; i ++)
        for (j = 0; j < self->width; j ++) {
            cur_distance += averageDistance(self, i, j, input);

            if (cur_distance < min_distance) {
                result.x = j;
                result.y = i;
                result.distance = cur_distance;
                min_distance = cur_distance;
            }

            cur_distance = 0.0;
        }

    return result;
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

KHNTraining khnNewTraining (
    KHNNet net,
    double neighbourhood,
    double larning_rate,
    double time_const
) {
    KHNTraining training_return = (KHNTraining)
        malloc(sizeof(struct KHNTraining_ST));

    training_return->cur_iter = 0;
    training_return->net = net;
    training_return->neighbourhood = neighbourhood;
    training_return->larning_rate = larning_rate;
    training_return->time_const = time_const;

    return training_return;
}

void khnDestroyTraining (KHNTraining* self_p) {
    free(*self_p);
    *self_p = NULL;
}

KHNTraining khnCopyTraining (const KHNTraining self) {
    KHNTraining copy = khnNewTraining(
        self->net, self->neighbourhood, self->larning_rate, self->time_const
    );

    copy->cur_iter = self->cur_iter;

    return copy;
}

KHNResult_ST khnTrainingIterate (KHNTraining self, const double input[]) {
    int i, j, k;
    double adjustment, larning_f, neighbourhood_f;
    KHNResult_ST result = khnGetResult(self->net, input);

    self->cur_iter ++;

    for (i = 0; i < self->net->input_len; i ++)
        for (j = 0; j < self->net->height; j ++)
            for (k = 0; k < self->net->width; k ++) {
                larning_f = self->larning_rate *
                    exp((double) self->cur_iter / self->time_const);

                neighbourhood_f = exp(
                    pow(distance(result.x, result.y, k, j), 2.0) /
                    2.0 * pow(
                        self->neighbourhood *
                        exp((double) self->cur_iter / self->time_const),
                    2.0)
                );

                adjustment = larning_f * neighbourhood_f *
                    (input[i] - self->net->grid[i][j][k]);

                self->net->grid[i][j][k] += adjustment;
            }


    return result;
}

/* ========================================================================== */

#endif

