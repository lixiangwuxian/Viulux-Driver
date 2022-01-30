#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "driverlog.h"

/** returns hmddisplay, hmdtracker, leftcontroller, rightcontroller */
int *get_configvalues() {
    int *vals = (int*) malloc(4*sizeof(int));
    vals[0] = 0;
    vals[1] = 0;
    vals[2] = -1;
    vals[3] = -1;
    return vals;
}
