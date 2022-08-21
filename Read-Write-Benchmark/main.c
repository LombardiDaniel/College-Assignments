#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
##include <unistd.h>


#define atoa(x) #x


FILE *createFile(int fileSize, char *fileName) {
    sprintf(fileName, "data/%d.txt\0", fileSize);

    // Open file
    return fopen(fileName, "wb");
}

void fillFile(FILE *file, int opSize) {
    for(int i=0; i<opSize; i++) {
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
    scanf("%d", &fileAmount);

    double sequentialReadAverage = 0;
    double randomReadAverage = 0;

    FILE *sequentialReadData = fopen("sequential_read_data.txt", "wb");

    for(int i=1; i<=fileAmount; i++) {
        opSize = i;

        // Write file
        FILE* file = createFile(opSize, fileName);
        fillFile(file, opSize);
        fclose(file);

        // Fill the struct (BENCHMARK SEQUENTIAL READ)
        timedFileOp sreadOperation;
        sreadOperation.elapsedTime = -1;
        sreadOperation.type = SEQUENTIAL_READ;
        sreadOperation.opSize = opSize;
        sreadOperation.fileName = fileName;

        benchmark(&sreadOperation);
        sequentialReadAverage += sreadOperation.elapsedTime;
        fprintf(sequentialReadData, "%f\n", sreadOperation.elapsedTime);

        // // Fill the struct (BENCHMARK RANDOM READ)
        // timedFileOp rreadOperation;
        // rreadOperation.elapsedTime = -1;
        // rreadOperation.type = RANDOM_READ;
        // rreadOperation.opSize = opSize;
        // rreadOperation.fileName = fileName;

        // benchmark(&rreadOperation);
        // randomReadAverage += rreadOperation.elapsedTime;
        // printf("Random Reading - Elapsed Time (s): %f\n", rreadOperation.elapsedTime);
    }

    sequentialReadAverage /= fileAmount;
    randomReadAverage /= fileAmount;

    fprintf(sequentialReadData, "Sequential Reading Average - Elapsed Time (s): %f\n", sequentialReadAverage);
    // printf("Random Reading Average - Elapsed Time (s): %f\n", randomReadAverage);

    fclose(sequentialReadData);

    return 0;
}
