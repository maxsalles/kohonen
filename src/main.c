#include <stdio.h>
#include "kohonen.h"

int main (void) {
    int i = 0;
    KHNNet net = khnNew(10, 10);
    KHNNet net_copy = khnCopy(net);

    khnPrint(net);
    khnPrint(net_copy);

    khnDestroy(&net);

    return 0;
}

