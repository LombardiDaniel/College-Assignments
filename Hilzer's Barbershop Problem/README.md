
## Hilzers Barbershop Problem


#### Problema em questão
> Our barbershop has three chairs, three barbers, and a waiting area that can accommodate four customers on a sofa and that has standing room for additional customers. Fire codes limit the total number of customers in the shop to 20.
>
> A customer will not enter the shop if it is filled to capacity with other customers. Once inside, the customer takes a seat on the sofa or stands if the sofa is filled. When a barber is free, the customer that has been on the sofa the longest is served and, if there are any standing customers, the one that has been in the shop the longest takes a seat on the sofa. When a customer’s haircut is finished, any barber can accept payment, but because there is only one cash register, payment is accepted for one customer at a time. The bar- bers divide their time among cutting hair, accepting payment, and sleeping in their chair waiting for a customer.

### Exemplo

No exemplo abaixo, temos uma versão simplificada, feita para exemplificação, do Hilzer's Barbershop Problem onde 2 barbeiros, cada um com uma cadeira, cortam o cabelo de 2 clientes numa barbearia com um sofá de 2 lugares.

Na Figura 1.1, dois clientes esperam no sofá e um espera em pé.

Na Figura 1.2, os dois barbeiros são liberados e os dois clientes sentados no sofá (FIFO) são liberados por meio do semáforo a sentar na cadeira de cada barbeiro. O cliente 3 também sai da sua fila e senta no sofá, liberado pelo semáforo do sofá.

Na Figura 1.3, o cliente 1 vai pagar e o barbeiro vai receber o dinheiro. O cliente 3 fica pronto.

Na Figura 1.4, o processo cliente 1 termina (Cliente faz o pagamento, barbeiro emite o recibo) e o barbeiro volta para sua cadeira, permitindo que ele atenda o cliente 3.

![Figure 1.1](11.png)

![Figure 1.2](12.png)

![Figure 1.3](13.png)

![Figure 1.4](14.png)


#### Breaking Down the Problem:

##### Entidades:
- Slots:
- 3 on barber chair's
- 4 on sofa
- 13 standing

##### Actions:
- Customers will sit on the sofa until it is filled, if filled, they stand (FIFO)
- The customer on the sofa for the longest time will be served first (FIFO)
- After the haircut, one customer at a time pays for it (Only one cash register), any barber can receive payments

##### Fluxo dos clientes:
```py
entrarNaLoja();
sentarNoSofa();
cortarCabelo();
sentarCadeiraBarbeiro();
pagar();
```

##### Fluxo dos barbeiros:
```py
cortarCabelo();
emitirRecibo();
```

#### Estruturas Necessárias:
Controle de fluxo:
```c
short qntClientes;

sem_t barberChairSemaphore; // start value @ 3
sem_t sofaSemaphore; // start value @ 4
// sem_t shopSemaphore; // start value @ 20
sem_t cashSemaphore; // start value @ 0
sem_t receiptSemaphore; // start value @ 0

pthread_mutex_t barberCashRegisterMutex;

unsigned sofaQueue[]; // holds IDs of customers
unsigned customerQueue[]; // holds IDs of customers

// Já que o primeiro a entrar será sempre o primeiro a sentar no sofá,
// não é necessário filas indivuais
// sofaQueue;
// chairQueue;
```

### Fluxo do Cliente

Inicialmente, o cliente verifica se a barbearia está cheia (20 clientes). Para isso, ele trava a execucao do programa e checa a variavel compartilhada `customers`. Caso a barbearia esteja cheia, o processo finaliza execucao. Caso contrário, ele incrementa a variavel:

```c
sem_wait(&mutex);

if(customers>=20) {
    sem_post(&mutex);
    printf("exiting shop %d.. \n",n);
}

customers++;
```

Ao entrar, o cliente sinaliza o semáforo e fica esperando sua vez na fila:
```c
entrarNaLoja();
sem_post(&mutex);
wait_Fifo(standingRoom,n);
```

Ao entrar na sala de espera, o cliente espera sua liberação para sentar no sofá:

```c
printf("Entering to standing room %d \n",n);
wait_Fifo(sofa,n);
```

Quando senta no sofá, o cliente novamente espera sua liberação para sentar na cadeira do barbeiro e sinaliza a fila de espera do sofá que ele vai sentar:

```c
printf("sitting in sofa %d \n", n);
sentarNoSofa();
signal_Fifo(standingRoom);
sem_wait(&chair);
```

Ao ser liberado para sentar na cadeira do barbeiro, o cliente sinaliza a fila do sofá e a fila de clientes, espera pelo barbeiro ser liberado e executa a função de cortar cabelo:

```c
printf("sit barb chair %d \n",n);
sentarCadeiraBarbeiro();
signal_Fifo(sofa);
sem_post(&customer);
sem_wait(&barber);
printf("Get hair Cut %d\n",n);
cortarCabelo();
```

Em seguida, o cliente realiza o pagamento e espera pelo recibo emitido pelo barbeiro:

```c
printf("pay %d \n", n);
pagar();
sem_post(&cash);
sem_wait(&reciept);
```

Por fim, o cliente decrementa a variável `customers` e termina a execução da thread:

```c
sem_wait(&mutex);
customers--;
sem_post(&mutex);
printf("exiting shop %d \n",n);
```

### Fluxo do Cliente

Ao iniciar a thread cutting (thread do barbeiro), o semáforo do cliente recebe um sinal `sem_wait()` para indicar que há um cliente para cortar o cabelo e logo em seguida o barbeiro recebe o sinal liberando-o para fazer o corte.

```c
sem_wait(&customer);
sem_post(&barber);
printf("\tbarber %d cutting hair\n", n);
sleep(3);
```

Ao finalizar o corte, o barbeiro precisa receber o pagamento, então o semáforo de pagamento é sinalizado para que o cliente consiga pagar, porém o semáforo da cadeira ainda não é liberado para que, enquanto o barbeiro estiver esperando o pagamento, outro cliente não sente na cadeira.

```c
sem_wait(&cash);
printf("\tbarber %d accepting payment\n", n);
sleep(1);
```

Por último, o barbeiro emite o recibo, assim o cliente pode ir embora da barbearia e a cadeira é liberada pelo semáforo para o próximo cliente da fila.

```c
sem_post(&reciept);
sem_post(&chair);
```
