#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <stdio.h>
#include <time.h>

#define MIN_SIZE                                                    2<<0
#define MAX_SIZE                                                    2<<20

#define SEQUENTIAL                                                  0b00
#define RANDOM                                                      0b01
#define READ                                                        0b00
#define WRITE                                                       0b10

#define SEQUENTIAL_WRITE                             (SEQUENTIAL | READ)
#define RANDOM_WRITE                                    (RANDOM | WRITE)
#define SEQUENTIAL_READ                             (SEQUENTIAL | WRITE)
#define RANDOM_READ                                      (RANDOM | READ)

#define BENCHMARK_QTY                                                500

typedef struct {
    double elapsedTime;
    unsigned char type;
    size_t fileSize;
    char *fileName;
} timedFileOp;


int benchmark(timedFileOp *op);

double _benchmarkSequentialWrite(size_t fileSize, char *fileName);
double _benchmarkRandomWrite(size_t fileSize, char *fileName);
double _benchmarkSequentialRead(size_t fileSize, char *fileName);
double _benchmarkRandomRead(size_t fileSize, char *fileName);


void swap(int *a, int *b);

#endif
