#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef sem_t semaphore;

#define down sem_wait
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
/**
 * @brief Hilo de filosofos
 *
 * @param arg (int*) Referencia al identificador
 * @return void* Valor de retorno del hilo (no usado)
 */
void *philosopher(void *arg);

int main(int argc, char const *argv[])
{
    pthread_t *t; // Arreglo de filosofos
    int *ids;     // Arreglo de identificadores de filosofos
    n = 0;
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
    //Indicar que la simulacion termino
    finished = 1;

    //Esperar que los hilos terminen
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
    }
    printf("Philosopher %d finished\n", i);
}