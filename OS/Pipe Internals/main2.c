#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/pipe.h>
#include <errno.h>
#include <fcntl.h>


int main() {
    int arr[5] = {0};

    mkfifo("testFifo", 0777);

    int fd = open("testFifo", O_RDONLY);

    for (size_t i = 0; i < 5; i++) {
        read(fd, &arr[i], sizeof(int));
        printf("[RCV] %d\n", arr[i]);
    }

    close(fd);
    return 0;
}
