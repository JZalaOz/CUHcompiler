#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "errors.h"

void checkIfAllocationFailed(void* allocation) {
    if (allocation == NULL) {
        throwOutOfMemory();
    }
}

void throwOutOfMemory() {
    textColorStdErr(ATTRIBUTE_BRIGHT, COLOR_RED);
    fprintf(stderr, "\nError: Out of Memory.\n\n");
    resetTextColor();
    exit(1);
}