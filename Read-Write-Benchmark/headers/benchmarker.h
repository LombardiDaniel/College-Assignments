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
#define RANDOM_WRITE                                     (RANDOM | READ)
#define SEQUENTIAL_READ                             (SEQUENTIAL | WRITE)
#define RANDOM_READ                                     (RANDOM | WRITE)

#define BENCHMARK_QTY                                                500

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

typedef struct _timedFileOpStruct {
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


#endif
