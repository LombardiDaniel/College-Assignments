#include "headers/benchmarker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN_SIZE                                                    2<<0
#define MAX_SIZE                                                    2<<20
#define FILE_NAME                                              "data.txt"


// #define atoa(x) #x

// FILE *createFile(int opSize, char *fileName) {
//     sprintf(fileName, "data_%d.txt", opSize);
//
//     // Open file
//     return fopen(fileName, "w");
// }


int main(int argc, char const *argv[]) {
    // Variable initialization
    // unsigned long opSize = atoi(argv[argc-1]);;
    // char fileName[1024] = "data.txt";
    // Write file
    // FILE* file = createFile(MAX_SIZE, fileName);
    FILE *fp = fopen(FILE_NAME, "w");

    for (size_t i = 0; i < MAX_SIZE; i++){
        // fprintf(fp, "@");
        // printf("%lu\n", i);
        write(fp, "@", 1);
    }
    fclose(fp);

    FILE *r_s_r = fopen("seq_read.txt", "w");
    FILE *r_s_w = fopen("seq_write.txt", "w");
    FILE *r_r_r = fopen("rand_read.txt", "w");
    FILE *r_r_w = fopen("rand_write.txt", "w");

    timedFileOp currOperation;
    currOperation.elapsedTime = -1;
    currOperation.opSize = MIN_SIZE;
    // currOperation.opSize = 8202;
    currOperation.fileName = (char *) FILE_NAME;

    printf("MAX_SIZE=%llu\n", MAX_SIZE);
    while (currOperation.opSize <= MAX_SIZE) {
        printf("%lu\n", currOperation.opSize);
        // printf("....\n");

        currOperation.type = SEQUENTIAL_READ;
        benchmark(&currOperation);
        fprintf(r_s_r, "%lu, %lf\n", currOperation.opSize, currOperation.elapsedTime);
        // printf("-...\n");

        currOperation.type = SEQUENTIAL_WRITE;
        benchmark(&currOperation);
        fprintf(r_s_w, "%lu, %lf\n", currOperation.opSize, currOperation.elapsedTime);
        // printf("--..\n");

        currOperation.type = RANDOM_READ;
        benchmark(&currOperation);
        fprintf(r_r_r, "%lu, %lf\n", currOperation.opSize, currOperation.elapsedTime);
        // printf("---.\n");

        currOperation.type = RANDOM_WRITE;
        benchmark(&currOperation);
        fprintf(r_r_w, "%lu, %lf\n", currOperation.opSize, currOperation.elapsedTime);
        // printf("----\n");m

        currOperation.opSize += 50;
    }

    fclose(r_s_r);
    fclose(r_s_w);
    fclose(r_r_r);
    fclose(r_r_w);

    return 0;
}
