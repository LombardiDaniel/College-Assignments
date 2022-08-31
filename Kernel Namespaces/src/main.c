// Daniel Lombardi - RA: 738340
//      gcc -Wall -pedantic -Wextra -O2 -g main.c -o main.out -lpthread

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>


#define BLACK                                      "\e[0;30m"
#define RED                                        "\e[0;31m"
#define GREEN                                      "\e[0;32m"
#define YELLOW                                     "\e[0;33m"
#define BLUE                                       "\e[0;34m"
#define MAGENTA                                    "\e[0;35m"
#define CYAN                                       "\e[0;36m"
#define WHITE                                      "\e[0;37m"
#define RESET                                       "\x1b[0m"
#define BOLD                                       "\033[;1m"


// static is visible to the file-only
static int shared = 0;

void *routine0() {
    printf("Thread ID: %ld\n", pthread_self());
    sleep(1);

    printf("X_no_sum: %d\n", shared);
    return NULL;
}

void *routine1() {
    shared++;
    printf("Thread ID: %ld\n", pthread_self());
    sleep(1);

    printf("X_sum: %d\n", shared);
    return NULL;
}


int main() {

    printf("main thread: %ld\n", pthread_self());

    pthread_t t1, t2;

    pthread_create(&t1, NULL, &routine0, NULL);
    pthread_create(&t2, NULL, &routine1, NULL);

    pthread_join(t1, NULL); // "wait for thread to finnish"
    pthread_join(t2, NULL); // evita que o programa chame a finalização antes
    // das threads terminarem a execução (oq finalizaria elas tbm)

    int pid = fork(); // parent process recieves pid of 0
    if (pid == -1)
        return 1;

    // printf("Process ID: %d\n", getpid());

    if (pid == 0) {
        printf("Parent process is: %d\n", getpid());
        shared++;
    } else {
        printf("GETTING BOTH PID::\n");
        printf("Child  PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
    }

    // sleep(1);

    printf("%d: %d\n",getpid(), shared);

    if (pid != 0)
        wait(NULL);

    return 0;
}
