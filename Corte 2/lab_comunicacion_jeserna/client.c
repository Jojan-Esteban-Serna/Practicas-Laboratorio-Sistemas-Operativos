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
    int server_socket_fd;

    // Conexion del cliente
    int client_socket_fd;

    // Dirección IPv4 del servidor
    struct sockaddr_in addr;

    // Buffer de lectura de la entrada estandar
    char read_buffer[BUFSIZ];
    // buffer para leer datos del socket del servidor
    char server_buffer[BUFSIZ];
    // Cantidad de bytes escritos
    ssize_t n_written;
    // cantidad de bytes leidos
    ssize_t nread;

    // Rellenar la estructura con ceros (NULL)
    memset(&act, 0, sizeof(struct sigaction));
    // Establecer el manejador de la señal

    act.sa_handler = handle_signal;

    // Instalar los manejadores de señal
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    printf("Server started\n");

    // 1. Crear un socket IPv4, flujo (stream)
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // 2. Conectarse al servidor (127.0.0.1)
    // 2.1 Configurar la direccion IPv4
    // Rellenar la estructura con ceros (los primeros 1024 los usa el OS)
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    // El servidor escucha en cualquier interfaz de red.
    if (inet_aton("127.0.0.1", &addr.sin_addr) != 1)
    { // conectarse a la direccion del servidor
        fprintf(stderr, "Unable to parse the address\n");
        exit(EXIT_FAILURE);
    }
    // bind
    if (connect(server_socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    } // Es sockadd in porque es IPv4

    // POST: conexion establecida
    while (!finished)
    {
        // rellenar el buffer con 0
        memset(read_buffer, 0, BUFSIZ);

        // leer de la entrada estandar
        fgets(read_buffer, BUFSIZ, stdin);
        if (strlen(read_buffer) > 0)
        {
            n_written = write(server_socket_fd, read_buffer, strlen(read_buffer));
            if(n_written <= 0){
                finished = 1;
                continue;
            }
            nread = read(server_socket_fd, server_buffer, BUFSIZ);
            printf("Got from server %ld bytes: %s\n", nread, server_buffer);
        }
    }

    // 4.1 Cerrar el socket del servidor
    close(server_socket_fd);

    close(client_socket_fd);

    printf("Server finished.\n");
    exit(EXIT_SUCCESS);
}

void handle_signal(int sig)
{
    printf("Signal %d received\n", sig);
    finished = 1;
}