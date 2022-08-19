#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define atoa(x) #x


FILE *createFile(int fileSize, char *fileName) {
    sprintf(fileName, "%d", fileSize);
    strcat(fileName, ".txt\0");

    // Open file
    return fopen(fileName, "wb");
}

void fillFile(FILE *file, int fileSize) {
    for(int i=0; i<fileSize; i++) {
        fprintf(file, "@");
    }
}

int main() {
    // Variable initialization
    int fileSize = -1;
    char fileName[10];
    scanf("%d", &fileSize);

    // Write file
    FILE* file = createFile(fileSize, fileName);
    fillFile(file, fileSize);
    fclose(file);

    // Fill the struct (BENCHMARK SEQUENTIAL READ)
    timedFileOp op;
    op.elapsedTime = -1;
    op.type = SEQUENTIAL_READ;
    op.fileSize = fileSize;
    op.fileName = fileName;

    benchmark(&op);

    printf("Sequencial Reading - Elapsed Time (s): %f\n", op.elapsedTime);

    return 0;
}
