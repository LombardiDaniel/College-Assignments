#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>

#include "queue.h"


<<<<<<< HEAD
#define BLACK                                             "\e[0;30m"
#define RED                                               "\e[0;31m"
#define GREEN                                             "\e[0;32m"
#define YELLOW                                            "\e[0;33m"
#define BLUE                                              "\e[0;34m"
#define MAGENTA                                           "\e[0;35m"
#define CYAN                                              "\e[0;36m"
#define WHITE                                             "\e[0;37m"
#define RESET                                             "\001b[0m"
#define BOLD                                              "\033[;1m"


#define N_BARBERS                                                  3
#define N_SOFA_SEATS                                               4
#define N_MAX_CUSTOMERS                                           20
#define N_COUNT                                                    6
#define N_MAX_EM_PE     (N_MAX_CUSTOMERS - N_SOFA_SEATS - N_BARBERS)
#define CADEIRA_OCUPADA                                           -1

#define SIM_PERIOD                                                 1

// typedef struct Queue_ {
//     QNode *front;
//     QNode *rear;
//
//     unsigned count;
// } Queue;
=======
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
#define N_COUNT                                            80
#define N_MAX_EM_PE (N_MAX_CUSTOMERS - N_SOFA_SEATS - N_BARBERS)
#define CADEIRA_OCUPADA                                    -1


#define SIM_PERIOD                                          1


// Structs
typedef struct {
    sem_t leader;
    sem_t follower;
} queue;
>>>>>>> a92f483d4a1766c6e7b7cc9880aad3787a454407


void *customerRoutine(void *args); // args[0] is client_id
void *barberRoutine(void *args); // args[0] is client_id

// Fluxo do cliente
void entrarNaLoja(unsigned id);
void sentarNoSofa(unsigned id);
void recebendoCorteCabelo(unsigned id);
void sentarCadeiraBarbeiro(unsigned id);
void pagar(unsigned id);


// Fluxo do barbeiro
void cortarCabelo(unsigned id);
void emitirRecibo(unsigned id);


// Abstrações:
void _sleep(unsigned seconds_avg);
void printCliente(char *str, unsigned id);


// Shared
short customerCount = 0;

// Control
sem_t barberChairSemaphore; // start value @ 3
sem_t sofaSemaphore; // start value @ 4
sem_t shopSemaphore; // start value @ 20

sem_t cashSemaphore; // start value @ 0
sem_t receiptSemaphore; // start value @ 0

sem_t barberSemaphore; // start value @ 0
sem_t customerSemaphore; // start value @ 0

pthread_mutex_t barberCashRegisterMutex;

Queue qRegistradora;
pthread_mutex_t mutexRegistradora;

Queue qSofa;
pthread_mutex_t mutexSofa;

Queue qEmPe;
pthread_mutex_t mutexEmPe;

// int cadeirasBarbeiros[3] = {-1};

// Fluxo Principal (Thread creation)
int main(int argc, char const *argv[]) {

    pthread_t thBarbers[N_BARBERS];
    pthread_t thCustomers[N_MAX_CUSTOMERS];

    pthread_mutex_init(&barberCashRegisterMutex, NULL);
    pthread_mutex_init(&mutexRegistradora, NULL);
    pthread_mutex_init(&mutexSofa, NULL);
    pthread_mutex_init(&mutexEmPe, NULL);
    sem_init(&barberChairSemaphore, 0, N_BARBERS);
    sem_init(&sofaSemaphore, 0, N_SOFA_SEATS);
    sem_init(&shopSemaphore, 0, N_MAX_CUSTOMERS);

    // começa em zero pq é transação (cliente_cabelo_cortado x barbeiro)
    sem_init(&cashSemaphore, 0, 0);
    sem_init(&receiptSemaphore, 0, 0);

    sem_init(&barberSemaphore, 0, 0);
    sem_init(&customerSemaphore, 0, 0);

    unsigned long customerId[6] = {0};
    // unsigned long barberId[3] = {0};

    for (size_t i = 0; i < N_COUNT; i++) {
        customerId[i] = i;
        pthread_create(&thCustomers[i], NULL, customerRoutine, &customerId[i]);
    }

    for (size_t i = 0; i < N_BARBERS; i++) {
        // barberId[i] = i;
        pthread_create(&thBarbers[i], NULL, barberRoutine, &customerId[i]);
    }

    printf("Thread creation successful\n");

    for (size_t i = 0; i < N_COUNT; i++) {
        pthread_join(thCustomers[i], NULL);
    }

    printf("Thread joining successful\n");

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
    long barberID = *(long *) args;
    printf("Inicializando rotina do barbeiro %ld\n", barberID);

    for (size_t i = 0; i < N_COUNT; i++) {
        // Realiza corte de cabelo do cliente
        // printf("Esperando pelo semaforo do cliente!\n");

        sem_wait(&customerSemaphore);
        sem_post(&barberSemaphore);
        cortarCabelo(barberID);

        // Espera o cliente pagar e emite recibo
        sem_wait(&cashSemaphore);
        sem_post(&receiptSemaphore);
        emitirRecibo(barberID);

        // Sinaliza o semaforo da cadeira (chair)
        sem_post(&barberChairSemaphore);
    }

    return NULL;
}


void *customerRoutine(void *args) {
    // unsigned long custID = *(unsigned long *) args;
    long custID = *(long *) args;
    printf("Inicializando rotina do cliente %d\n", custID);

    // checa vagas de pé
    pthread_mutex_lock(&mutexEmPe);
    // printf("A fila em pe possui %d pessoas\n", qEmPe.count);
    if (qEmPe.count >= N_MAX_EM_PE) {
        printf("Numero maximo de clientes excedido!\n");
        pthread_mutex_unlock(&mutexEmPe);
        pthread_exit(NULL);
    }

    // entra na loja
    entrarNaLoja(custID);
    enqueue(&qEmPe, custID);
    pthread_mutex_unlock(&mutexEmPe);

    // checa vagas no sofa
    printf("O cliente %ld ve que o sofa possui %d pessoas\n", custID, qSofa.count);
    sem_wait(&sofaSemaphore);
    sentarNoSofa(custID);
    dequeue(&qEmPe);
    enqueue(&qSofa, custID);

    // checa vagas do barbeiro
    sem_wait(&barberChairSemaphore);
    dequeue(&qSofa);
    sem_post(&sofaSemaphore);
    sentarCadeiraBarbeiro(custID);

    // espera por barbeiro
    sem_post(&customerSemaphore);
    sem_wait(&barberSemaphore);
    recebendoCorteCabelo(custID);

    // realiza pagamento
    sem_post(&cashSemaphore);
    sem_wait(&receiptSemaphore);
    pagar(custID);

    // sai da loja
    // pthread_mutex_lock(&mutexRegistradora);
    // printf("Tchau\n");
    // pthread_mutex_unlock(&mutexRegistradora);
    return NULL;
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

void recebendoCorteCabelo(unsigned id) {
    _sleep(rand() % 5);
    printf("[%sCLIENTE%s]O cliente %u recebeu o corte de cabelo.\n", GREEN, WHITE, id);
}

void sentarCadeiraBarbeiro(unsigned id) {
    _sleep(0);
    printf("[%sCLIENTE%s]O cliente %u sentou na cadeira do barbeiro.\n", GREEN, WHITE, id);
}

void pagar(unsigned id) {
    _sleep(0);
    printf("[%sCLIENTE%s]O cliente %u pagou.\n", GREEN, WHITE, id);
}


// Fluxo do barbeiro
void cortarCabelo(unsigned id) {
    _sleep(0);
    printf("[%sBARBEIRO%s]O barbeiro %d cortou o cabelo de um cliente.\n", GREEN, WHITE, id);
}

void emitirRecibo(unsigned id) {
    _sleep(0);
    printf("[%sBARBEIRO%s]O barbeiro %d emitiu o recibo de um cliente.\n", GREEN, WHITE, id);
}

// Abstrações
void _sleep(unsigned seconds_avg) {
    float tmp = seconds_avg + rand() % unsigned(seconds_avg * 1.1);
    sleep(tmp * SIM_PERIOD);
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
<<<<<<< HEAD
=======

// Fluxo Principal (Thread creation)
int main() {
    pthread_t thBarbers[N_BARBERS];
    pthread_t thCustomers[N_MAX_CUSTOMERS];
    int i;

    pthread_mutex_init(&barberCashRegisterMutex, NULL);
    pthread_mutex_init(&mutexRegistradora, NULL);
    pthread_mutex_init(&mutexSofa, NULL);
    pthread_mutex_init(&mutexEmPe, NULL);
    sem_init(&barberChairSemaphore, 0, N_BARBERS);
    sem_init(&sofaSemaphore, 0, N_SOFA_SEATS);
    sem_init(&shopSemaphore, 0, N_MAX_CUSTOMERS);

    // começa em zero pq é transação (cliente_cabelo_cortado x barbeiro)
    sem_init(&cashSemaphore, 0, 0);
    sem_init(&receiptSemaphore, 0, 0);

    sem_init(&barberSemaphore, 0, 0);
    sem_init(&customerSemaphore, 0, 0);

    unsigned long customerId[80] = {0};
    // unsigned long barberId[3] = {0};

    for (i=0; i<N_COUNT; i++) {
        customerId[i] = i;
        pthread_create(&thCustomers[i], NULL, customerRoutine, &customerId[i]);
    }

    for (i=0; i<N_BARBERS; i++) {
        // barberId[i] = i;
        pthread_create(&thBarbers[i], NULL, barberRoutine, &customerId[i]);
    }

    printf("Thread creation successful\n");

    for (i=0; i<N_COUNT; i++) {
        pthread_join(thCustomers[i], NULL);
    }

    printf("Thread joining successful\n");

    // unsigned long customerId[88] = {0};
    //
    // for (size_t i = 0; i < 6; i++) {
    //     customerId[i] = i;
    //     pthread_create(&thCustomers[i], NULL, barberRoutine, &customerId[i]);
    // }

    // pthread_join(t2, NULL);


    return 0;
}
>>>>>>> a92f483d4a1766c6e7b7cc9880aad3787a454407
