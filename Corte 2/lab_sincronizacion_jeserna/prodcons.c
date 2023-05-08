#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
/**
 * @brief Definicion del tipo semaforo
 *
 */
typedef sem_t semaphore;

#define N 10 // TODO: tamaño del buffer por linea de comando
semaphore mutex; //Se inicializan con sem_init
semaphore empty;//Se inicializan con sem_init
semaphore full;//Se inicializan con sem_init
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
    pthread_t t_prod, t_cons;//Identificadores de hilos

    // Inicializar los semaforos
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    finished = 0;
    // Crear el hilo productor
    pthread_create(&t_prod, NULL, producer, NULL);

    // Crear el hilo consumidor
    pthread_create(&t_cons, NULL, consumer, NULL);

    // Dormir durante 10 segundos para dar tiempo a que se ejecuten los hilos
    sleep(2);

    //Notificar que se debe terminar
    finished = 1;

    //Esperar que los hilos terminen
    pthread_join(t_prod, NULL);
    pthread_join(t_cons, NULL);

    exit(EXIT_SUCCESS);
}

int produce_item()
{
    printf(" P ");
    fflush(stdout);
    usleep(rand()%1000);
    return -1;
}

void insert_item(int item)
{
    printf(" I ");
    fflush(stdout);
}
int remove_item()
{
    printf(" R ");
    fflush(stdout);
    return -1;
}
void consume_item(int item)
{
    printf(" C ");
    fflush(stdout);
    usleep(rand()%1000);

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