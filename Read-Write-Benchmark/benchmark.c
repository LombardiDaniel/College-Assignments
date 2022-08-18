#include <stdio.h>
#include <time.h>

int main() {

    clock_t begin = clock();

    printf("Test elapsed time: \n");

    clock_t end = clock();
    double elapsed_secs = (double) (end - begin) / CLOCKS_PER_SEC;

    printf("Elapsed Time (s): %f\n", elapsed_secs);
    return 0;
}