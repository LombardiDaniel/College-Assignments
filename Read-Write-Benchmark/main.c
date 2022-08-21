#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN_SIZE                                                    2<<0
#define MAX_SIZE                                                    2<<20


#define atoa(x) #x


FILE *createFile(int opSize, char *fileName) {
    sprintf(fileName, "data_%d.txt\0", opSize);

    // Open file
    return fopen(fileName, "wb");
}

void fillFile(FILE *file, long long int fileSize) {
    printf("File size: %lld\n", fileSize);

    for(int i=0; i<fileSize; i++) {
        fprintf(file, "@");
    }
    // fwrite(file, "@", 1, fileSize, 0);
}


int main(int argc, char const *argv[]) {
    // Variable initialization
    unsigned long opSize = atoi(argv[argc-1]);;
    char fileName[1024];
    int sequentialReadTime, randomReadTime;


    // Write file
    FILE* file = createFile(MAX_SIZE, fileName);
    fillFile(file, MAX_SIZE);
    fclose(file);

    FILE *r_s_r = fopen("seq_read.txt", "w");
    FILE *r_s_w = fopen("seq_write.txt", "w");
    FILE *r_r_r = fopen("rand_read.txt", "w");
    FILE *r_r_w = fopen("rand_write.txt", "w");

    timedFileOp currOperation;
    currOperation.elapsedTime = -1;
    currOperation.opSize = opSize;
    currOperation.fileName = fileName;

    while (currOperation.opSize <= MAX_SIZE) {

        currOperation.type = SEQUENTIAL_READ;
        benchmark(&currOperation);
        fprintf(r_s_r, "%lu, %lf\n", opSize, currOperation.elapsedTime);

        currOperation.type = SEQUENTIAL_WRITE;
        benchmark(&currOperation);
        fprintf(r_s_w, "%lu, %lf\n", opSize, currOperation.elapsedTime);

        currOperation.type = RANDOM_READ;
        benchmark(&currOperation);
        fprintf(r_r_r, "%lu, %lf\n", opSize, currOperation.elapsedTime);

        currOperation.type = RANDOM_WRITE;
        benchmark(&currOperation);
        fprintf(r_r_w, "%lu, %lf\n", opSize, currOperation.elapsedTime);

        currOperation.opSize += 50;
    }


    // printf("Sequential Reading - Elapsed Time (s): %f\n", sreadOperation.elapsedTime);
    //
    // // // // Fill the struct (BENCHMARK RANDOM READ)
    // timedFileOp rreadOperation;
    // rreadOperation.elapsedTime = -1;
    // rreadOperation.type = RANDOM_READ;
    // rreadOperation.opSize = opSize;
    // rreadOperation.fileName = fileName;
    //
    // benchmark(&rreadOperation);
    // randomReadTime = rreadOperation.elapsedTime;
    // fwrite(results, 2 * sizeof(int), 1, FILE *restrict stream);
    // tenq escrever os dados num arquivo de texto, pra ficar assim:
    // tam, optype, time\n
    // 300, 0011, double(time)\n
    // printf("Random Reading - Elapsed Time (s): %f\n", rreadOperation.elapsedTime);

    fclose(r_s_r);
    fclose(r_s_w);
    fclose(r_r_r);
    fclose(r_r_w);

    return 0;
}
