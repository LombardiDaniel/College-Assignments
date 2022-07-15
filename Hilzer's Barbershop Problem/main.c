// Run with: ./main.out NUMBER_OF_CLIENT_THREADS
//
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>

#include "queue.h"


#define BLACK                                                "\e[0;30m"
#define RED                                                  "\e[0;31m"
#define GREEN                                                "\e[0;32m"
#define YELLOW                                               "\e[0;33m"
#define BLUE                                                 "\e[0;34m"
#define MAGENTA                                              "\e[0;35m"
#define CYAN                                                 "\e[0;36m"
#define WHITE                                                "\e[0;37m"
#define RESET                                                 "\x1b[0m"
#define BOLD                                                 "\033[;1m"


#define N_BARBERS                                                     3
#define N_SOFA_SEATS                                                  4
#define N_MAX_CUSTOMERS                                              20
#define N_MAX_EM_PE        (N_MAX_CUSTOMERS - N_SOFA_SEATS - N_BARBERS)
#define CADEIRA_OCUPADA                                              -1


#define SIM_FREQ                                                    1


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
void _sleep(unsigned seconds);
void printCliente(char *str, unsigned id);
void printBarbeiro(char *str, unsigned id);

unsigned long CLIENT_AMMOUNT = 0;
unsigned long CLIENT_AMMOUNT_GAVE_UP = 0;


// Control
sem_t barberChairSemaphore; // start value @ 3
sem_t sofaSemaphore; // start value @ 4
// sem_t shopSemaphore; // start value @ 20 -> not used because clients don't wait

sem_t cashSemaphore; // start value @ 0
sem_t receiptSemaphore; // start value @ 0

sem_t barberReadySemaphore; // start value @ 3
sem_t customerReadySemaphore; // start value @ 0

pthread_mutex_t barberCashRegisterMutex;

Queue qRegistradora;
pthread_mutex_t mutexRegistradora;

Queue qSofa;
pthread_mutex_t mutexSofa;

Queue qEmPe;
pthread_mutex_t mutexEmPe;


int main(int argc, char *argv[]) {

    CLIENT_AMMOUNT = atoi(argv[argc-1]);

    pthread_t thBarbers[N_BARBERS];
    pthread_t thCustomers[N_MAX_CUSTOMERS];
    long *customerId = (long *) malloc(sizeof(unsigned long) * CLIENT_AMMOUNT);
    short barberId[N_BARBERS];

    pthread_mutex_init(&barberCashRegisterMutex, NULL);
    pthread_mutex_init(&mutexRegistradora, NULL);
    pthread_mutex_init(&mutexSofa, NULL);
    pthread_mutex_init(&mutexEmPe, NULL);
    sem_init(&barberChairSemaphore, 0, N_BARBERS);
    // sem_init(&barberChairSemaphore, 0, 0);
    sem_init(&sofaSemaphore, 0, N_SOFA_SEATS);

    // começa em zero pq é transação (cliente_cabelo_cortado x barbeiro)
    sem_init(&cashSemaphore, 0, 0);
    sem_init(&receiptSemaphore, 0, 0);

    sem_init(&barberReadySemaphore, 0, 3);
    sem_init(&customerReadySemaphore, 0, 0);


    for (size_t i = 0; i < N_BARBERS; i++) {
        barberId[i] = i;
        pthread_create(&thBarbers[i], NULL, barberRoutine, &barberId[i]);
    }

    for (size_t i = 0; i < CLIENT_AMMOUNT; i++) {
        sleep(rand() % 2);
        customerId[i] = i;
        pthread_create(&thCustomers[i], NULL, customerRoutine, &customerId[i]);
    }

    printf("[INFO]::All threads created.\n");

    for (size_t i = 0; i < CLIENT_AMMOUNT; i++) {
        pthread_join(thCustomers[i], NULL);
    }

    printf("[INFO]::All threads joined.\n");
    printf("[INFO]::%ld/%ld clients did not cut their hair today. :(\n", CLIENT_AMMOUNT_GAVE_UP, CLIENT_AMMOUNT);


    // destoy
    pthread_mutex_destroy(&barberCashRegisterMutex);
    pthread_mutex_destroy(&mutexRegistradora);
    pthread_mutex_destroy(&mutexSofa);
    pthread_mutex_destroy(&mutexEmPe);
    sem_destroy(&barberChairSemaphore);
    sem_destroy(&sofaSemaphore);

    sem_destroy(&cashSemaphore);
    sem_destroy(&receiptSemaphore);

    sem_destroy(&barberReadySemaphore);
    sem_destroy(&customerReadySemaphore);

    return 0;
}


void *barberRoutine(void *args) {

    short barberID = *(short *) args;

    printf("[INFO]::Inicializando rotina do barbeiro %d\n", barberID);

    while (1) {
    // for (size_t i = 0; i < CLIENT_AMMOUNT; i++) {
        // Realiza corte de cabelo do cliente

        // no costumerRoutine:
            // // espera por barbeiro
            // sem_post(&customerReadySemaphore);
            // sem_wait(&barberReadySemaphore);
            // recebendoCorteCabelo(custID);

        sem_post(&barberReadySemaphore);
        sem_wait(&customerReadySemaphore);
        cortarCabelo(barberID);
        sem_post(&barberChairSemaphore);


        // Espera o cliente pagar e emite recibo
        sem_wait(&cashSemaphore);
        pthread_mutex_lock(&mutexRegistradora);
        emitirRecibo(barberID);
        sem_post(&receiptSemaphore);
        pthread_mutex_unlock(&mutexRegistradora);
    }

    return NULL;
}


void *customerRoutine(void *args) {
    // unsigned long custID = *(unsigned long *) args;
    long custID = *(long *) args;
    // printf("Inicializando rotina do cliente %ld\n", custID);

    // checa vagas de pé
        pthread_mutex_lock(&mutexEmPe);
    // printf("A fila em pe possui %d pessoas\n", qEmPe.count);
    if (qEmPe.count >= N_MAX_EM_PE) { // não utiliza um semaforo pq ele nao espera abrir, se n cabe, desiste
        printf("[INFO]::Numero maximo de clientes excedido!\n");
        printCliente("Desistiu de cortar o cabelo.", custID);
        CLIENT_AMMOUNT_GAVE_UP++;
        pthread_mutex_unlock(&mutexEmPe);
        pthread_exit(NULL);
    }

    // entra na loja
    entrarNaLoja(custID);
    enqueue(&qEmPe, custID);
    pthread_mutex_unlock(&mutexEmPe);


    // espera vaga no sofa
    sem_wait(&sofaSemaphore);

    pthread_mutex_lock(&mutexEmPe);
        dequeue(&qEmPe);
        pthread_mutex_lock(&mutexSofa);
            enqueue(&qSofa, custID);
            sentarNoSofa(custID); // print
        pthread_mutex_unlock(&mutexSofa);
    pthread_mutex_unlock(&mutexEmPe); // só nao está mais de pé quando senta no sofá


    // checa vagas do barbeiro
    sem_wait(&barberChairSemaphore);

    pthread_mutex_lock(&mutexSofa);
        dequeue(&qSofa);
        sentarCadeiraBarbeiro(custID); // print
    pthread_mutex_unlock(&mutexSofa);
    sem_post(&sofaSemaphore); // só libera o sofá quando senta na cadeira do barbeiro


    // espera por barbeiro
    sem_wait(&barberReadySemaphore);
    sem_post(&customerReadySemaphore);
    recebendoCorteCabelo(custID);


    // realiza pagamento
    sem_post(&cashSemaphore);
    sem_wait(&receiptSemaphore);
    pagar(custID);

    // sai da loja

    return NULL;
}


// Fluxo do cliente
void entrarNaLoja(unsigned id) {
    _sleep(0);
    printCliente("Entrou na loja.", id);
}

void sentarNoSofa(unsigned id) {
    _sleep(0);
    printCliente("Sentou no sofá.", id);
}

void sentarCadeiraBarbeiro(unsigned id) {
    _sleep(0);
    printCliente("Sentou na cadeira do Barbeiro.", id);
}

void recebendoCorteCabelo(unsigned id) {
    _sleep(0);
    printCliente("Cortou o cabelo.", id);
}

void pagar(unsigned id) {
    _sleep(0);
    printCliente("Pagou.", id);
}


// Fluxo do barbeiro
void cortarCabelo(unsigned id) {
    _sleep(0);
    printBarbeiro("Cortou o cabelo de um cliente.", id);
}

void emitirRecibo(unsigned id) {
    _sleep(0);
    printBarbeiro("Emitiu um recibo.", id);
}


// Abstrações
void _sleep(unsigned seconds_avg) {
    if (seconds_avg == 0)
        return;

    seconds_avg *= 1000000; // to microseconds

    float tmp = seconds_avg + rand() % (unsigned) (seconds_avg*1.5);
    usleep(tmp / SIM_FREQ);
}

void printCliente(char *str, unsigned id) {
    char COLORS[5][8] = {
        // BLACK,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN
    };

    unsigned c = id % 5;
    printf("[%sCLIENTE::%u%s] %s\n", &COLORS[c][0], id, RESET, str);
}

void printBarbeiro(char *str, unsigned id) {
    printf("[%sBARBEIRO::%u%s] %s\n", RED, id, RESET, str);
}
