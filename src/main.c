#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "common.h"
#include "lexer.h"
#include "errors.h"

char* readSourceFile(char* filePath) {
    printf("Reading %s..\n", filePath);

    int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening input file: %s\n", strerror(errno));
        exit(1);
    }

    off_t fileSize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char* buffer = malloc(fileSize + 1);
    checkIfAllocationFailed(buffer);

    ssize_t bytesRead = read(fd, buffer, fileSize);
    if (bytesRead == -1) {
        fprintf(stderr, "Error reading input file: %s\n", strerror(errno));
        free(buffer);
        close(fd);
        exit(1);
    }

    buffer[fileSize] = '\0';
    close(fd);

    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        textColorStdErr(ATTRIBUTE_BRIGHT, COLOR_RED);
        fprintf(stderr, "Error: No input file provided.\n");
        textColorStdErr(ATTRIBUTE_RESET, COLOR_GREEN);
        fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
        resetTextColorStdErr();
        exit(1);
    }

    char* rawCode = readSourceFile(argv[1]);
    LexerResult lexerResult = tokenize(strlen(rawCode), rawCode);

    for (int i = 0; i < lexerResult.tokenC; i++) {
        printf("%s - %d \n", lexerResult.tokenV[i].value, lexerResult.tokenV[i].type);
    }

    printf("\n");
    return 0;
}