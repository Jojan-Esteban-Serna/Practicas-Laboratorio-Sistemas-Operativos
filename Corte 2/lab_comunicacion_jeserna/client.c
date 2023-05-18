#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief Manejador de señal.
 *
 * @param sig Numero de la señal recibida
 */
void handle_signal(int sig);

/**
 * @brief Controla la terminación del programa
 *
 */
int finished = 0;

int main(int argc, char const *argv[])
{
    // Estructura para manejar la señal
    struct sigaction act;

    // Socket del servidor
    int s;

    // Dirección IPv4 del servidor
    struct sockaddr_in addr;

    // Rellenar la estructura con ceros (NULL)
    memset(&act, 0, sizeof(struct sigaction));
    // Establecer el manejador de la señal

    act.sa_handler = handle_signal;

    // Instalar los manejadores de señal
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    printf("Server started\n");

    // 1. Crear el conector
    s = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Conectarse al servidor (127.0.0.1)
    // 2.1 Configurar la direccion IPv4
    // Rellenar la estructura con ceros (los primeros 1024 los usa el OS)
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    // El servidor escucha en cualquier interfaz de red.
    inet_aton("127.0.0.1", &addr.sin_addr);

    connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)); // Es sockadd in porque es IPv4


    while (!finished)
    {
    }

    // 4.1 Cerrar el socket del servidor
    close(s);

    printf("Server finished.\n");
    exit(EXIT_SUCCESS);
}

void handle_signal(int sig)
{
    printf("Signal %d received\n", sig);
    finished = 1;
}