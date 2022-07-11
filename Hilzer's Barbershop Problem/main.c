#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>


#define N_BARBERS                                 3
#define N_SOFA_SEATS                              4
#define N_MAX_CUSTOMERS                           20
#define N_COUNT                                   88


// Structs
typedef struct {
    sem_t leader;
    sem_t follower;
} queue;

// Shared
short customerCount = 0;
// fifo *sofa, *emPe;

// Control
sem_t barberChairSemaphore; // start value @ 3
sem_t sofaSemaphore; // start value @ 4
sem_t shopSemaphore; // start value @ 20

sem_t cashSemaphore; // start value @ 0
sem_t receiptSemaphore; // start value @ 0

pthread_mutex_t barberCashRegisterMutex;


void *custumerRoutine(void *args); // args[0] is client_id
void *barberRoutine(void *args); // args[0] is client_id

// fifo *fifo_init(int n);


int main() {
    pthread_t thBarbers[N_BARBERS];
    pthread_t thCustomers[N_MAX_CUSTOMERS];

    pthread_mutex_init(&barberCashRegisterMutex, NULL);
    sem_init(&barberChairSemaphore, 0, N_BARBERS);
    sem_init(&sofaSemaphore, 0, N_SOFA_SEATS);
    sem_init(&shopSemaphore, 0, N_MAX_CUSTOMERS);

    // começa em zero pq é transação (cliente_cabelo_cortado x barberiro)
    sem_init(&cashSemaphore, 0, 0);
    sem_init(&receiptSemaphore, 0, 0);


    unsigned long customerId[88] = {0};

    for (size_t i = 0; i < 6; i++) {
        customerId[i] = i;
        pthread_create(&thCustomers[i], NULL, barberRoutine, &customerId[i]);
    }

    // pthread_join(t2, NULL);

    return 0;
}


void *barberRoutine(void *args) {
    // unsigned long custID = *(unsigned long *) args;
    long custID = *(long *) args;



}
