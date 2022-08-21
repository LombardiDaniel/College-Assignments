#include "headers/benchmarker.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int benchmark(timedFileOp *op) {

    switch (op->type) {
        case SEQUENTIAL_WRITE:
            op->elapsedTime = _benchmarkSequentialWrite(
                op->opSize,
                op->fileName
            );
            break;

        case SEQUENTIAL_READ:
            op->elapsedTime = _benchmarkSequentialRead(
                op->opSize,
                op->fileName
            );
            break;

        case RANDOM_WRITE:
            op->elapsedTime = _benchmarkRandomWrite(
                op->opSize,
                op->fileName
            );
            break;

        case RANDOM_READ:
            op->elapsedTime = _benchmarkRandomRead(
                op->opSize,
                op->fileName
            );
            break;

    }

    return 0;
}


double _benchmarkSequentialWrite(size_t opSize, char *fileName) {
    FILE *fd = fopen(fileName, "wb");

    clock_t start_time = clock();
    size_t amount = write(fd, '-', opSize);
    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    fclose(fd);

    if (amount != opSize)
        return -1.f;

    return elapsed_time;
}


double _benchmarkSequentialRead(size_t opSize, char *fileName) {
    FILE *fd = fopen(fileName, "rb");

    unsigned char *byteBuff = 0;

    clock_t start_time = clock();

    for (size_t i = 0; i < opSize; i++) {
        size_t amount = read(fd, byteBuff, 1);
    }

    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    fclose(fd);

    return elapsed_time;
}


double _benchmarkRandomWrite(size_t opSize, char *fileName) {

    // we first create a list containing the access order of the file,
    // then we shuffle it and follow the new pseudo-random order
    size_t *bytesReadOrder = (size_t *) malloc(opSize * sizeof(int));

    for (size_t i = 0; i < opSize; i++)
        bytesReadOrder[i] = i;

    for (size_t i = 0; i < opSize; i++) {
        int other_i = rand() % opSize;
        swap(&bytesReadOrder[i], &bytesReadOrder[other_i]);
    }

    // Benchmarking starts:
    FILE *fd = fopen(fileName, "rwb");

    unsigned char *byteBuff = 0;

    clock_t start_time = clock();

    for (size_t i = 0; i < opSize; i++) {
        size_t amount = pwrite(fd, "#", 1, bytesReadOrder[i]);
    }

    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    fclose(fd);

    return elapsed_time;

}


double _benchmarkRandomRead(size_t opSize, char *fileName) {

    // we first create a list containing the access order of the file,
    // then we shuffle it and follow the new pseudo-random order
    size_t *bytesReadOrder = (size_t *) malloc(opSize * sizeof(int));

    for (size_t i = 0; i < opSize; i++)
        bytesReadOrder[i] = i;

    for (size_t i = 0; i < opSize; i++) {
        int other_i = rand() % opSize;
        swap(&bytesReadOrder[i], &bytesReadOrder[other_i]);
    }

    // Benchmarking starts:
    FILE *fd = fopen(fileName, "rb");

    unsigned char *byteBuff = 0;
    clock_t start_time = clock();

    for (size_t i = 0; i < opSize; i++) {
        size_t amount = pread(fd, byteBuff, 1, bytesReadOrder[i]);
    }

    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    fclose(fd);

    return elapsed_time;

}


void swap(unsigned *a, unsigned *b) {
    int *tmp = a;
    a = b;
    b = tmp;
}
