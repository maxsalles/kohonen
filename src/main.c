#include <stdio.h>
#include "kohonen.h"

int main (void) {
    double input[] = { 2.0 };
    KHNNet net = khnNew(2, 2, 1);
    KHNResult_ST result;

    khnPrint(net);
    result = khnGetResult(net, input);

    printf("(%i, %i, %f)", result.x, result.y, result.distance);

    return 0;
}

