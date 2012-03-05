#include <stdio.h>
#include "kohonen.h"

int main (void) {
    double input[] = { 2.0, 2.0 };
    KHNNet net = khnNew(2, 2, 2);
    KHNTraining training = khnNewTraining(net, 0.5, 0.5, 4.0);

    khnPrint(net);

    khnTrainingIterate(training, input);

    khnPrint(net);

    return 0;
}

