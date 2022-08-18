#include <stdio.h>
#include <time.h>

void readFile(char *filePath) {
    int fd, flags = 0;
    char c;

    fd = open(filePath, flags);

    if(fd == -1) {
        printf("Erro na abertura do arquivo: %s nao encontrado.\n", filePath);
        return;
    }

    while(read(fd, &c, 1)) {
        printf("%c", c);
    }
    printf("\n\n");

    return;
}

void preadFile(FILE *file) {
    return;
}

int main() {

    clock_t begin = clock();

    // Code goes here
    readFile("foo.txt");

    clock_t end = clock();
    double elapsed_secs = (double) (end - begin) / CLOCKS_PER_SEC;

    printf("Elapsed Time (s): %f\n", elapsed_secs);
    return 0;
}