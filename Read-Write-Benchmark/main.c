#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


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

unsigned long long getTotalSystemMemory() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);

    return pages * page_size;
}

int main(int argc, char const *argv[]) {
    // Variable initialization
    int opSize;
    long long int fileSize = 2 * getTotalSystemMemory();
    char fileName[1024];
    int sequentialReadTime, randomReadTime;

    opSize = atoi(argv[argc-1]);
    // printf("Operation size: \n");
    // scanf("%d", &opSize);

    FILE *results = fopen("results.txt", "w");
    // Write file
    FILE* file = createFile(opSize, fileName);
    fillFile(file, fileSize);
    fclose(file);

    // Fill the struct (BENCHMARK SEQUENTIAL READ)
    timedFileOp sreadOperation;
    sreadOperation.elapsedTime = -1;
    sreadOperation.type = SEQUENTIAL_READ;
    sreadOperation.opSize = opSize;
    sreadOperation.fileName = fileName;

    benchmark(&sreadOperation);
    sequentialReadTime = sreadOperation.elapsedTime;
    printf("Sequential Reading - Elapsed Time (s): %f\n", sreadOperation.elapsedTime);

    // // // Fill the struct (BENCHMARK RANDOM READ)
    timedFileOp rreadOperation;
    rreadOperation.elapsedTime = -1;
    rreadOperation.type = RANDOM_READ;
    rreadOperation.opSize = opSize;
    rreadOperation.fileName = fileName;

    benchmark(&rreadOperation);
    randomReadTime = rreadOperation.elapsedTime;
    // fwrite(results, 2 * sizeof(int), 1, FILE *restrict stream);
    // tenq escrever os dados num arquivo de texto, pra ficar assim:
    // tam, optype, time\n
    // 300, 0011, double(time)\n
    printf("Random Reading - Elapsed Time (s): %f\n", rreadOperation.elapsedTime);

    fclose(results);

    return 0;
}
