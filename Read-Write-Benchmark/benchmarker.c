#include "headers/benchmarker.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// #include <time.h>
//
// int main(void) {
//   clock_t start_time = clock();
//
//   // code or function to benchmark
//
//   double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
//   printf("Done in %f seconds\n", elapsed_time);
// }


int benchmark(timedFileOp *op) {

    switch (op->type) {
        case SEQUENTIAL_WRITE:
            op->elapsedTime = _benchmarkSequentialWrite(
                op->fileSize,
                op->fileName
            );
            break;

        case SEQUENTIAL_READ:
            op->elapsedTime = _benchmarkSequentialRead(
                op->fileSize,
                op->fileName
            );
            break;

    }

    return 0;
}


double _benchmarkSequentialWrite(size_t fileSize, char *fileName) {
    FILE *fd = fopen(fileName, 'w');

    clock_t start_time = clock();
    size_t ammount = write(fd, '-', fileSize);
    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    if (ammount != fileSize)
        return -1.f;

    return elapsed_time;
}


double _benchmarkSequentialRead(size_t fileSize, char *fileName) {
    FILE *fd = fopen(fileName, 'w');

    unsigned char *byteBuff = 0;

    clock_t start_time = clock();

    for (size_t i = 0; i < fileSize; i++) {
        size_t ammount = read(fd, byteBuff, 1);
    }

    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    return elapsed_time;
}

double _benchmarkRandomWrite(size_t fileSize, char *fileName);
double _benchmarkRandomRead(size_t fileSize, char *fileName);
