#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <stdio.h>
#include <time.h>

#define MIN_SIZE                                                    2<<0
#define MAX_SIZE                                                    2<<20

// int main(void) {
//   clock_t start_time = clock();
//
//   // code or function to benchmark
//
//   double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
//   printf("Done in %f seconds\n", elapsed_time);
// }


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

typedef struct {
    double elapsedTime;
    unsigned char type;
    size_t fileSize;
} timedFileOp;


int benchmarkRun(void *nome);



#endif
