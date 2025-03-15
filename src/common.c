#include <stdio.h>

#include "common.h"

void textColor(int attribute, int color) {
    char command[7];
    sprintf(command, "\x1B[%d;%dm", attribute, color + 30);
	printf("%s", command);
}

void textColorStdErr(int attribute, int color) {
    char command[9];
    sprintf(command, "\x1B[%d;%dm", attribute, color + 30);
	fprintf(stderr, "%s", command);
}

void resetTextColor() {
    printf("\x1B[0m");
}

void resetTextColorStdErr() {
    fprintf(stderr, "\x1B[0m");
}

int minInt(int a, int b) {
    return (a < b) ? a : b;
}

int maxInt(int a, int b) {
    return (a > b) ? a : b;
}