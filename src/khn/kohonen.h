/*
** $ Herond Robaina Salles
** Prefix: khn
*/

#ifndef _KHN_LIB
#define _KHN_LIB

/* ========================================================================== */

struct KHNNet_ST;

struct KHNTraining_ST;

typedef struct {
    int x, y;
    double distance;
} KHNResult_ST;

typedef struct KHNTraining_ST* KHNTraining;

typedef struct KHNNet_ST* KHNNet;

/* ========================================================================== */

KHNNet khnNew (unsigned width, unsigned height, unsigned input_len);

void khnDestroy (KHNNet* self_p);

KHNNet khnCopy (KHNNet self);

void khnClear (KHNNet self);

KHNResult_ST khnGetResult (KHNNet self, const double input[]);

void khnPrint (const KHNNet self);

/* ========================================================================== */

KHNTraining khnNewTraining (
    KHNNet net,
    double neighbourhood,
    double larning_rate,
    double time_const
);

void khnDestroyTraining (KHNTraining* self_p);

KHNTraining khnCopyTraining (const KHNTraining self);

KHNResult_ST khnTrainingIterate (KHNTraining self, const double input[]);

/* ========================================================================== */

#endif

