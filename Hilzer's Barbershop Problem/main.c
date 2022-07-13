#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>

#include "queue.h"


#define BLACK                                      "\e[0;30m"
#define RED                                        "\e[0;31m"
#define GREEN                                      "\e[0;32m"
#define YELLOW                                     "\e[0;33m"
#define BLUE                                       "\e[0;34m"
#define MAGENTA                                    "\e[0;35m"
#define CYAN                                       "\e[0;36m"
#define WHITE                                      "\e[0;37m"
#define RESET                                      "\001b[0m"
#define BOLD                                       "\033[;1m"


#define N_BARBERS                                           3
#define N_SOFA_SEATS                                        4
#define N_MAX_CUSTOMERS                                    20
#define N_COUNT                                            88
#define N_MAX_EM_PE (N_MAX_CUSTOMERS - N_SOFA_SEATS - N_BARBERS)
#define CADEIRA_OCUPADA                                    -1


#define SIM_PERIOD                                          1


// Structs
typedef struct {
    sem_t leader;
    sem_t follower;
} queue;


void *customerRoutine(void *args); // args[0] is client_id
void *barberRoutine(void *args); // args[0] is client_id

// Fluxo do cliente
void entrarNaLoja(unsigned id);
void sentarNoSofa(unsigned id);
void cortarCabelo(unsigned id);
void sentarCadeiraBarbeiro(unsigned id);
void pagar(unsigned id);


// Fluxo do barberiro
void cortarCabelo(unsigned id);
void emitirRecibo(unsigned id);


// Abstrações:
void _sleep(unsigned seconds);
void printCliente(char *str, unsigned id);


// Shared
short customerCount = 0;

// Control
sem_t barberChairSemaphore; // start value @ 3
sem_t sofaSemaphore; // start value @ 4
sem_t shopSemaphore; // start value @ 20

sem_t cashSemaphore; // start value @ 0
sem_t receiptSemaphore; // start value @ 0

pthread_mutex_t barberCashRegisterMutex;


Queue qRegistradora;
pthread_mutex_t mutexRegistradora;

Queue qSofa;
pthread_mutex_t mutexSofa;

Queue qEmPe;
pthread_mutex_t mutexEmPe;


int cadeirasBarbeiros[3] = {-1};


int main() {
    pthread_t thBarbers[N_BARBERS];
    pthread_t thCustomers[N_MAX_CUSTOMERS];

    pthread_mutex_init(&barberCashRegisterMutex, NULL);
    pthread_mutex_init(&mutexRegistradora, NULL);
    pthread_mutex_init(&mutexSofa, NULL);
    pthread_mutex_init(&mutexEmPe, NULL);
    sem_init(&barberChairSemaphore, 0, N_BARBERS);
    sem_init(&sofaSemaphore, 0, N_SOFA_SEATS);
    sem_init(&shopSemaphore, 0, N_MAX_CUSTOMERS);

    // começa em zero pq é transação (cliente_cabelo_cortado x barberiro)
    sem_init(&cashSemaphore, 0, 0);
    sem_init(&receiptSemaphore, 0, 0);


    // unsigned long customerId[88] = {0};
    //
    // for (size_t i = 0; i < 6; i++) {
    //     customerId[i] = i;
    //     pthread_create(&thCustomers[i], NULL, barberRoutine, &customerId[i]);
    // }

    // pthread_join(t2, NULL);


    return 0;
}


void *barberRoutine(void *args) {
    // unsigned long custID = *(unsigned long *) args;
    long custID = *(long *) args;



}


void *customerRoutine(void *args) {
    // unsigned long custID = *(unsigned long *) args;
    long custID = *(long *) args;

    // checa vagas de pé
    pthread_mutex_lock(&mutexEmPe);
    if (qEmPe.count >= N_MAX_EM_PE) {
        pthread_mutex_unlock(&mutexEmPe);
        pthread_exit(NULL);
    }

    enqueue(&qEmPe, custID);
    pthread_mutex_unlock(&mutexEmPe);


    // chega vagas sentados
    sem_wait(&sofaSemaphore);
    dequeue(&qEmPe);
    enqueue(&qSofa, custID);


    // chega vagas do barberiro -> precisa criar estrutura das cadeiras (lista[3] = id ? id : -1)
    sem_wait(&barberChairSemaphore);

}



// Fluxo do cliente
void entrarNaLoja(unsigned id) {
    _sleep(0);
    printf("[%sCLIENTE%s]O cliente %u entrou na loja.\n", GREEN, WHITE, id);
}

void sentarNoSofa(unsigned id) {
    _sleep(0);
    printf("[%sCLIENTE%s]O cliente %u sentou no sofa.\n", GREEN, WHITE, id);
}

void cortarCabelo(unsigned id) {
    _sleep(rand() % 5);
    printf("[%sCLIENTE%s]O cliente %u cortou o cabelo.\n", GREEN, WHITE, id);
}

void sentarCadeiraBarbeiro(unsigned id) {
    _sleep(0);
    printf("[%sCLIENTE%s]O cliente %u sentou na cadeira do barberiro.\n", GREEN, WHITE, id);
}

void pagar(unsigned id);


// Fluxo do barberiro
void cortarCabelo(unsigned id);
void emitirRecibo(unsigned id);

// Abstrações
void _sleep(unsigned seconds) {
    sleep(seconds * SIM_PERIOD);
}

void printCliente(char *str, unsigned id) {
    char COLORS[7][9] = {
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN
    };

    unsigned c = id % 7;
    printf("[%sCLIENTE::%u%s] %s\n", &COLORS[c][0], id, RESET, str);
}
