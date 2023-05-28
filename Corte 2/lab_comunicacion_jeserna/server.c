#include <arpa/inet.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"
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

    // Socket del cliente
    int client_socket_fd;

    // Dirección IPv4 del servidor
    struct sockaddr_in server_address;
    //Buffer de lectura
    char read_buffer[BUFSIZ];
    ssize_t nread;
    //cantidad de bytes escritos
    ssize_t nwritten;
    // Direccion del cliente
    struct sockaddr_in client_address;
    socklen_t c_len;

    // Rellenar la estructura con ceros (NULL)
    memset(&act, 0, sizeof(struct sigaction));
    // Establecer el manejador de la señal

    act.sa_handler = handle_signal;

    // Instalar los manejadores de señal
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    printf("Server started\n");

    // 1. Crear el socket IPV4, flujo (stream)
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket creado\n");
    // 2. Asociar el socket con una dirección
    // 2.1 Configurar la direccion IPv4
    // Rellenar la estructura con ceros (los primeros 1024 los usa el OS)
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(1234);
    // El servidor escucha en cualquier interfaz de red.
    if(inet_aton("0.0.0.0", &server_address.sin_addr) != 1){
        fprintf(stderr, "Unable to parse the address\n");
        exit(EXIT_SUCCESS);
    }

    if(bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in))!= 0){
        perror("bind");
        exit(EXIT_FAILURE);
    } // Es sockadd in porque es IPv4
    printf("Socket asociado a una direccion");
    // 3. Poner el socket disponible (escuchar)
    if(listen(server_socket_fd, 10)!= 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 4. Esperar por una conexion
    client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_address, &c_len);

    if(client_socket_fd <0){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while (!finished)
    {
        nread = read(client_socket_fd, read_buffer, BUFSIZ);
        if(nread <= 0){
            finished = 1;
            continue;
        }
        printf("data read (%d): %s\n",nread, read_buffer);

        to_upper(read_buffer);
        nwritten = write(client_socket_fd, read_buffer, BUFSIZ);
        if(nwritten <= 0){
            finished = 1;
            continue;
        }
    }

    // 4. cerrar la conexion con el cliente
    close(client_socket_fd);
    // 4.1 Cerrar el socket del servidor
    close(server_socket_fd);

    printf("Server finished.\n");
    exit(EXIT_SUCCESS);
}

void handle_signal(int sig)
{
    printf("Signal %d received\n", sig);
    finished = 1;
}