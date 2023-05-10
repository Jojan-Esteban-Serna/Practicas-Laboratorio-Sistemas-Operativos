#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
/**
 * @brief Definicion del tipo semaforo
 *
 */
typedef sem_t semaphore;

semaphore mutex; // Se inicializan con sem_init
semaphore empty; // Se inicializan con sem_init
semaphore full;  // Se inicializan con sem_init
int *buffer;     // Buffer de productos
int n;           // Tamaño del buffer

int head = 0; // Cabeza de la lista circular
int tail = 0; // Cola de la lista circular
/**
 * @brief Alias para sem_wait
 *
 */

#define down sem_wait

/**
 * @brief Alias para sem_post
 *
 */
#define up sem_post

/**
 * @brief Hilo productor
 *
 * @param arg Argumento al hilo (no usado)
 * @return void* Valor de retorno (no usado)
 */
void *producer(void *arg);

/**
 * @brief Hilo consumidor
 *
 * @param arg Argumento al hilo (no usado)
 * @return void* Valor de retorno (no usado)
 */
void *consumer(void *arg);

/**
 * @brief Produce un nuevo item
 *
 * @return int Nuevo item
 */
int produce_item();

/**
 * @brief Inserta un nuevo item en el buffer
 *
 * @param item Item a insertar
 */
void insert_item(int item);

/**
 * @brief Obtiene y remueve un item del buffer
 *
 * @return int Item obtenido
 */
int remove_item();
/**
 * @brief Imprime el buffer circular, señalando el inicio y el final con `|`
 * 
 * @param buffer Buffer a imprimir
 */
void print_circular_buffer(int *buffer);
/**
 * @brief Consume un item
 *
 * @param item Item a consumir
 */
void consume_item(int item);

/**
 * @brief Bandera que indica el fin de la simulacion
 *
 */
int finished;

int main(int argc, char const *argv[])
{

    pthread_t t_prod, t_cons; // Identificadores de hilos

    time_t t;
    srand((unsigned)time(&t)); // Semilla con el tiempo

    finished = 0;
    while (n <= 0)
    {
        // Nota: Si se hace "./prodcons > out.txt", el siguiente mensaje no se mostrara
        // Pero si leera por teclado lo que se ingrese
        printf("Ingrese el tamaño del buffer\n>");
        scanf("%d", &n);
    }
    // Inicializar los semaforos
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, n);
    sem_init(&full, 0, 0);
    buffer = (int *)calloc(n, sizeof(int));
    // Crear el hilo productor
    pthread_create(&t_prod, NULL, producer, NULL);

    // Crear el hilo consumidor
    pthread_create(&t_cons, NULL, consumer, NULL);

    // Dormir durante 2 segundos para dar tiempo a que se ejecuten los hilos
    sleep(2);

    // Notificar que se debe terminar
    finished = 1;

    // Esperar que los hilos terminen
    pthread_join(t_prod, NULL);
    pthread_join(t_cons, NULL);

    // Liberar la memoria del buffer
    free(buffer);
    exit(EXIT_SUCCESS);
}

int produce_item()
{

    int item = n + rand() % n * n;
    printf(" Produced: %d\n", item);
    fflush(stdout);
    usleep(rand() % 1000);
    return item;
}

void insert_item(int item)
{
    buffer[head] = item;
    head = (head + 1) % n;
    printf(" Inserted: %d \n", item);
    print_circular_buffer(buffer);
    fflush(stdout);
}
int remove_item()
{
    int item = buffer[tail];
    buffer[tail] = 0;
    tail = (tail + 1) % n;
    printf(" Removed: %d \n", item);
    print_circular_buffer(buffer);

    fflush(stdout);
    return item;
}
void print_circular_buffer(int *buffer)
{
    printf("[");
    for (size_t i = 0; i < n; i++)
    {
        if (i == tail || i == head)
        {
            printf("|");
        }
        printf(" %d ", buffer[i]);
    }
    printf("]\n");
}
void consume_item(int item)
{
    printf(" Consumed: %d \n", item);
    fflush(stdout);
    usleep(rand() % 1000);
}
void *producer(void *arg)
{
    int item;
    while (!finished)
    {
        item = produce_item(); // Producir un nuevo item
        down(&empty);          // Bloquear en el semaforo empty
        down(&mutex);          // Inicio de la seccion critica para insertar
        insert_item(item);     // Colocar el item en el buffer
        up(&mutex);            // Fin de la seccion critica para insertar
        up(&full);             // Desbloquear el semaforo full
    }
}
void *consumer(void *arg)
{
    int item;
    while (!finished)
    {
        down(&full);              // Bloquear en el semaforo full (Cero = bloqueo)
        down(&mutex);             // Iniciar la seccion critica para sacar el item
        item = remove_item(item); // Sacar el item del buffer
        up(&mutex);               // Salir de la seccion critica
        up(&empty);               // Desbloquear el semaforo empty
        consume_item(item);       // Consumir el item obtenido
    }
}