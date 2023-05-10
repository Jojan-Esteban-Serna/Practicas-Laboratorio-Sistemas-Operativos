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
 * @brief Indice de la izquierda de i (en recorrido circular)
 *
 */
#define LEFT(i) (i - 1 + n) % n
/**
 * @brief Indice de la derecha de i (en recorrido circular)
 *
 */
#define RIGHT(i) (i + 1) % n
/**
 * @brief Bandera para indicar el fin de
 *
 */
int finished;
/**
 * @brief Mutex
 *
 */
semaphore mutex; // Se inicializan con sem_init
/**
 * @brief Arreglo de semaforos
 *
 */
semaphore *semaphores;

/**
 * @brief Definicion del tipo semaforo
 *
 */
typedef sem_t semaphore;

/**
 * @brief Estado de un filosofo
 *
 */
typedef enum
{
    HUNGRY,
    THINKING,
    EATING
} State;

/**
 * @brief Arreglo de estados
 *
 */
State *state;

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

/**
 * @brief Funcion llamada mientras se piensa
 *
 * @param i Indice del filosofo que piensa
 */
void think(int i);
/**
 * @brief Funcion llamada mientras se come
 *
 * @param i Indice del filosofo pensante
 */
void eat(int i);

/**
 * @brief Imprime el arreglo de estados (Sirve para comprobar que no pueden haber filosofos adyacentes comiendo)
 *
 * @param states Arreglo de estados
 * @param n tamaño del arreglo
 */
void print_states(State *states, int n);
int main(int argc, char const *argv[])
{
    pthread_t *philosophers; // Arreglo de filosofos
    int *ids;                // Arreglo de identificadores de filosofos
    n = 0;                   // Numero de filosofos
    sem_init(&mutex, 0, 1);

    while (n < 2)
    {
        printf("Ingrese la cantidad de filosofos:");
        scanf("%d", &n);
    }
    finished = 0;
    // Reservar memoria para el arreglo de hilos
    philosophers = (pthread_t *)malloc(n * sizeof(pthread_t));

    // Reservar memoria para el arreglo de identificadores
    ids = (int *)malloc(n * sizeof(int));

    // Reservar memoria para el arreglo de semaforos
    semaphores = (semaphore *)malloc(n * sizeof(semaphore));

    // Reservar memoria para el arreglo de estados
    state = (State *)malloc(n * sizeof(State));

    // Inicializar los semaforos en cero y los estados en thinking
    for (size_t i = 0; i < n; i++)
    {
        sem_init(&semaphores[i], 0, 0);
        state[i] = THINKING;
    }
    print_states(state, n);
    // Crear los hilos
    for (int i = 0; i < n; i++)
    {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, (void *)&ids[i]);
    }
    sleep(5);
    // Indicar que la simulacion termino
    finished = 1;

    // Esperar que los hilos terminen
    for (int i = 0; i < n; i++)
    {
        pthread_join(philosophers[i], NULL);
    }
    // Liberar memoria de los arreglos creados dinamicamente
    free(philosophers);
    free(ids);
    free(semaphores);
    free(state);
    exit(EXIT_SUCCESS);
}

void *philosopher(void *arg)
{
    int i;

    i = *(int *)arg;
    printf("Philosopher %d started\n", i);
    fflush(stdout);

    while (!finished)
    {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
    printf("Philosopher %d finished\n", i);
    fflush(stdout);
}

void take_forks(int i)
{
    down(&mutex);
    state[i] = HUNGRY;
    print_states(state, n);
    test(i);
    up(&mutex);
    down(&semaphores[i]);
}
void put_forks(int i)
{
    down(&mutex);
    state[i] = THINKING;
    print_states(state, n);
    test(LEFT(i));
    test(RIGHT(i));
    up(&mutex);
}
void test(int i)
{
    if (state[i] == HUNGRY && state[LEFT(i)] != EATING && state[RIGHT(i)] != EATING)
    {
        state[i] = EATING;
        print_states(state, n);
        up(&semaphores[i]);
    }
}

void think(int i)
{
    printf("Philosopher %d is thinking\n", i);
    fflush(stdout);
}
void eat(int i)
{
    printf("Philosopher %d is eating\n", i);
    fflush(stdout);
}

void print_states(State *states, int n)
{
    printf("[");
    for (size_t i = 0; i < n; i++)
    {
        switch (states[i])
        {
        case THINKING:
            printf(" T "); // Significa thinking (Pensando)
            break;
        case HUNGRY:
            printf(" H "); // Significa hungry (Hambriento)
            break;

        case EATING:
            printf(" E "); // Significa eating (Comiendo)
            break;

        default:
            break;
        }
    }
    printf("]\n");
}