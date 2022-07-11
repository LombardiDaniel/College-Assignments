## Hilzer's Barbershop Problem

#### Problema em questão
> Our barbershop has three chairs, three barbers, and a waiting area that can accommodate four customers on a sofa and that has standing room for additional customers. Fire codes limit the total number of customers in the shop to 20.
>
> A customer will not enter the shop if it is filled to capacity with other customers. Once inside, the customer takes a seat on the sofa or stands if the sofa is filled. When a barber is free, the customer that has been on the sofa the longest is served and, if there are any standing customers, the one that has been in the shop the longest takes a seat on the sofa. When a customer’s haircut is finished, any barber can accept payment, but because there is only one cash register, payment is accepted for one customer at a time. The bar- bers divide their time among cutting hair, accepting payment, and sleeping in their chair waiting for a customer.


#### Breaking Down the Problem:

##### Entidades:
- Slots:
    - 3 on barber chair's
    - 4 on sofa
    - 13 standing

- Actions:
    - Customers will sit on the sofa untill it is filled, if filled, they stand (FIFO)
    - The customer on the sofa for the longest will be served first (FIFO)
    - After haircut, a customer pays one at a time (only one register), any barber can recieve payments

##### Fluxo dos clientes:
```py
    entrarNaLoja();
    sentarNoSsofa();
    cortarCabelo();
    pagar();
    receberRecibo();
```

##### Fluxo dos barbeiros:
```py
    cortarCabelo();
    receberPagamentoEmitirRecibo();
```

#### Estruturas Necessárias:

Controle de fluxo:

```c
short qntClientes;

sem_t barberChairSemaphore; // start value @ 3
sem_t sofaSemaphore; // start value @ 4
sem_t shopSemaphore; // start value @ 20

sem_t cashSemaphore; // start value @ 0
sem_t receiptSemaphore; // start value @ 0

pthread_mutex_t barberCashRegisterMutex;

unsigned sofaQueue[]; // holds IDs of customers
unsigned paymentQueue[]; // holds IDs of customers

// Já que o primeiro a entrar será sempre o primeiro a sentar no sofá,
//      não é necessário filas indivuais
// sofaQueue;
// chairQueue;
```
