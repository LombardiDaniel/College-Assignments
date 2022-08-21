#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define atoa(x) #x


FILE *createFile(int fileSize, char *fileName) {
    sprintf(fileName, "data/%d.txt\0", fileSize);

    // Open file
    return fopen(fileName, "wb");
}

void fillFile(FILE *file) {
    int fileSize = 2 * getTotalSystemMemory();
    for(int i=0; i<fileSize; i++) {
        fprintf(file, "@");
    }
}

unsigned long long getTotalSystemMemory() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);

    return pages * page_size;
}

int main() {
    // Variable initialization
    int fileAmount, opSize;
    char fileName[10];

    printf("Operation Size: \n");
    scanf("%d", &opSize);

    double sequentialReadTime = 0;
    double randomReadTime = 0;

    // Write file
    FILE* file = createFile(opSize, fileName);
    fillFile(file);
    fclose(file);

    // Fill the struct (BENCHMARK SEQUENTIAL READ)
    timedFileOp sreadOperation;
    sreadOperation.elapsedTime = -1;
    sreadOperation.type = SEQUENTIAL_READ;
    sreadOperation.opSize = opSize;
    sreadOperation.fileName = fileName;

    benchmark(&sreadOperation);
    printf("Random Reading - Elapsed Time (s): %f\n", sreadOperation.elapsedTime);

    // Fill the struct (BENCHMARK RANDOM READ)
    timedFileOp rreadOperation;
    rreadOperation.elapsedTime = -1;
    rreadOperation.type = RANDOM_READ;
    rreadOperation.opSize = opSize;
    rreadOperation.fileName = fileName;

    benchmark(&rreadOperation);
    printf("Random Reading - Elapsed Time (s): %f\n", rreadOperation.elapsedTime);

    return 0;
}
