#include <stdio.h>
#include "kohonen.h"

int main (void) {
    KHNNet net = khnNew(10, 10, 2);
    KHNNet net_copy = khnCopy(net);

    khnPrint(net);
    khnPrint(net_copy);

    khnDestroy(&net);

    return 0;
}

