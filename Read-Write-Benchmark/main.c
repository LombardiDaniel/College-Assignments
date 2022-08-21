#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define atoa(x) #x


FILE *createFile(int fileSize, char *fileName) {
    sprintf(fileName, "data/%d.txt\0", fileSize);

    // Open file
    return fopen(fileName, "wb");
}

void fillFile(FILE *file, int fileSize) {
    for(int i=0; i<fileSize; i++) {
        fprintf(file, "@");
    }
}

int main() {
    // Variable initialization
    int fileAmount, fileSize;
    char fileName[10];
    scanf("%d", &fileAmount);

    double sequentialReadAverage = 0;
    double randomReadAverage = 0;

    FILE *sequentialReadData = fopen("sequential_read_data.txt", "wb");

    for(int i=1; i<=fileAmount; i++) {
        fileSize = i;

        // Write file
        FILE* file = createFile(fileSize, fileName);
        fillFile(file, fileSize);
        fclose(file);

        // Fill the struct (BENCHMARK SEQUENTIAL READ)
        timedFileOp sreadOperation;
        sreadOperation.elapsedTime = -1;
        sreadOperation.type = SEQUENTIAL_READ;
        sreadOperation.fileSize = fileSize;
        sreadOperation.fileName = fileName;

        benchmark(&sreadOperation);
        sequentialReadAverage += sreadOperation.elapsedTime;
        fprintf(sequentialReadData, "%f\n", sreadOperation.elapsedTime);

        // // Fill the struct (BENCHMARK RANDOM READ)
        // timedFileOp rreadOperation;
        // rreadOperation.elapsedTime = -1;
        // rreadOperation.type = RANDOM_READ;
        // rreadOperation.fileSize = fileSize;
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
