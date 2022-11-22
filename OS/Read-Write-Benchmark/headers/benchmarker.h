#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <stdio.h>
#include <time.h>

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
    size_t opSize;
    char *fileName;
} timedFileOp;


int benchmark(timedFileOp *op);

double _benchmarkSequentialWrite(size_t opSize, char *fileName);
double _benchmarkRandomWrite(size_t opSize, char *fileName);
double _benchmarkSequentialRead(size_t opSize, char *fileName);
double _benchmarkRandomRead(size_t opSize, char *fileName);


void swap(unsigned *a, unsigned *b);

#endif
