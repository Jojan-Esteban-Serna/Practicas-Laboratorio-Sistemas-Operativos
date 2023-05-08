#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/**
 * @brief Definicion del tipo semaforo
 *
 */
typedef sem_t semaphore;

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
 * @brief Cantidad de filosofos
 *
 */

int n;
/**
 * @brief Bandera para indicar el fin de
 *
 */
int finished;

semaphore mutex; // Se inicializan con sem_init

/**
 * @brief Definicion del tipo semaforo
 *
 */
typedef sem_t semaphore;

/**
 * @brief Estado de un filosofo
 *
 */
enum State
{
    HUNGRY,
    THINKING,
    EATING
};

/**
 * @brief Hilo de filosofos
 *
 * @param arg (int*) Referencia al identificador
 * @return void* Valor de retorno del hilo (no usado)
 */
void *philosopher(void *arg);

/**
 * @brief Toma los tenedores
 *
 * @param i Indice del filosofo que va a tomar los tenedores
 */
void take_forks(int i);
/**
 * @brief Pone los tenedores
 *
 * @param i indice del filosofo que va a poner los tenedores
 */
void put_forks(int i);

/**
 * @brief Chequea si los filosofos de al lado no estan comiendo
 *
 * @param i Indice del filosofo que chequea
 */
void test(int i);

int main(int argc, char const *argv[])
{
    pthread_t *t; // Arreglo de filosofos
    int *ids;     // Arreglo de identificadores de filosofos
    n = 0;        // Numero de filosofos
    sem_init(&mutex, 0, 1);

    // Validar la cantidad de filosofos
    if (argc == 2)
    {
        n = atoi(argv[1]);
    }
    while (n < 2)
    {
        printf("Ingrese la cantidad de filosofos:");
        scanf("%d", &n);
    }
    finished = 0;
    // Reservar memoria para el arreglo de hilos
    t = (pthread_t *)malloc(n * sizeof(pthread_t));

    // Reservar memoria para el arreglo de identificadores
    ids = (int *)malloc(n * sizeof(int));

    // Crear los hilos
    for (int i = 0; i < n; i++)
    {
        ids[i] = i;
        pthread_create(&t[i], NULL, philosopher, (void *)&ids[i]);
    }
    sleep(5);
    // Indicar que la simulacion termino
    finished = 1;

    // Esperar que los hilos terminen
    for (int i = 0; i < n; i++)
    {
        pthread_join(t[i], NULL);
    }

    exit(EXIT_SUCCESS);
}

void *philosopher(void *arg)
{
    int i;

    i = *(int *)arg;
    printf("Philosopher %d started\n", i);
    while (!finished)
    {
        down(&mutex);
        state[i] = HUNGRY;
        test(i);
        up(&mutex);
        down(&s[i]);
    }
    printf("Philosopher %d finished\n", i);
}

void take_forks(int i)
{
    down(&mutex);
    state[i] = HUNGRY;
    test(i);
    up(&mutex);
    down(&s[i]);
}
void put_forks(int i)
{
    down(&mutex);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    up(&mutex);
}
void test(int i)
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[i] = EATING;
        up(&s[i]);
    }
}