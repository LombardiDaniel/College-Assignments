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


void benchmark();

double _benchmarkSequentialWrite(size_t fileSize, char *fileName) {
    FILE *fd = fopen(fileName, 'w');

    clock_t start_time = clock();
    size_t ammount = write(fd, '-', fileSize);
    double elapsed_time = (double) (clock() - start_time) / CLOCKS_PER_SEC;

    if (ammount != fileSize)
        return -1.f;

    return elapsed_time;
}

double _benchmarkSequentialWrite(size_t fileSize, char *fileName);
double _benchmarkRandomRead(size_t fileSize, char *fileName);
double _benchmarkSequentialRead(size_t fileSize, char *fileName);
