#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>

#define atoa(x) #x


int main() {
    int fileSize = -1;
    scanf("%d", &fileSize);

    char fileName[10];
    FILE *file;

    sprintf(fileName, "%d", fileSize);
    strcat(fileName, ".txt");

    // Open file
    file = fopen(fileName, "wb");

    // Fill with n '@'s
    for(int i=0; i<fileSize; i++) {
        fprintf(file, "@");
    }

    // Fill the struct
    timedFileOp op;
    op.elapsedTime = -1;


    return 0;
}