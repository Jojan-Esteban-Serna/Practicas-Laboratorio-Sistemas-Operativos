
/**
 * @file
 * @brief Servidor HTTP
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 */

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#include "util.h"
#include "split.h"
/**
 * @brief Manejador de señales.
 * @param num Numero de la señal recibida
 * @note El manejador debe ser instalado mediante sigaction(2).
 */
void signal_handler(int num);

/** @brief Controla la ejecucion del servidor */
int finished = 0;

int main(int argc, char *argv[])
{
#pragma region Configuraciones
	// Socket del servidor
	int s;

	// Conexion del cliente
	int client_socketfd;

	// Direccion IPv4 del servidor
	struct sockaddr_in addr;

	// Estructura de accion de señal
	struct sigaction act;

	// Buffer de lectura
	char read_buf[BUFSIZ];
	ssize_t nread;
	ssize_t nwritten;

	// Inicializar la estructura con ceros (NULL)
	memset(&act, 0, sizeof(struct sigaction));

	// Establecer el manejador de la señal
	act.sa_handler = signal_handler;

	// Instalar el manejador de la señal SIGINT
	sigaction(SIGINT, &act, NULL);

	// Instalar el manejador de la señal SIGTERM
	sigaction(SIGTERM, &act, NULL);

	// 1. Crear un socket IPv4, flujo (stream)
	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s < 0)
	{
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	// Inicializar la direccion antes de asociarla
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234); // \TODO argumento de linea de comandos!!!
	if (inet_aton("0.0.0.0", &addr.sin_addr) != 1)
	{ // Escuchar en la direccion 	ip QUE SE ME HAYA ASIGNADO
		fprintf(stderr, "Unable to parse the address\n");
		exit(EXIT_FAILURE);
	}

	// 2. Asociar el socket a la direccion addr de tipo IPv4  (bind)
	if (bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	// 3 Poner el socket disponible para recibir conexiones
	if (listen(s, 10) != 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("Server started.\n");

	// 4. Esperar (BLOQUEARSE) por una conexion
	client_socketfd = accept(s, NULL, 0); // \TODO opcional: recibir la direccion del cliente
	if (client_socketfd < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
#pragma endregion
	/* TODO crear un hilo que realice la siguiente tarea: */

	while (!finished)
	{
		// Leer de la conexion del cliente
		nread = read(client_socketfd, read_buf, BUFSIZ);
		if (nread > 0)
		{
			// La solicitud tiene formato GET /ruta_archivo HTTP/1.1\n\n
			printf("Client sended: %s\n", read_buf);

			// separar la solicitud en partes
			split_list *parts = split(read_buf, " ");
			// Verificar que la solicitud tenga el formato correcto
			if (parts->count != 3)
			{
				finished = 1;
				continue;
			}
			// Verificar que la solicitud sea GET
			if (!equals(parts->parts[0], "GET"))
			{
				finished = 1;
				continue;
			}
			// Verificar que la solicitud ttenga http/1.1
			if (!equals(parts->parts[2], "HTTP/1.1\n\n"))
			{
				finished = 1;
				continue;
			}
			char *filename = parts->parts[1];
			char path[BUFSIZ];
			sprintf(path, "www%s", filename);
			printf("Archivo solicitado: %s\n", path);
			//  Buscar el archivo dentro del subdirectorio con (stat) www/
			struct stat stat_buffer;
			int stat_result = stat(path, &stat_buffer);
			if (stat_result < 0)
			{
				printf("Archivo no encontrado\n");
				// Si no existe, construir el mensaje de error (HTTP/1.1 404 Not Found) y enviar
				char html_not_found_response[BUFSIZ];
				sprintf(html_not_found_response,
						"<!DOCTYPE html>\n"
						"<html lang=\"en\">\n"
						"<head>\n"
						"<meta charset=\"utf-8\">\n"
						"<title>Error</title>\n"
						"</head>\n"
						"<body>\n"
						"<pre>Cannot GET %s</pre>\n"
						"</body>\n"
						"</html>",
						path);
				char header_html_not_found_response[BUFSIZ];
				time_t current_time;
				time(&current_time);

				printf("%lu\n", current_time);

				printf("%s\n", asctime(localtime(&current_time)));

				// Convert the time to a string representation
				char *time_string = ctime(&current_time);

				sprintf(header_html_not_found_response,
						"HTTP/1.1 404 Not Found\n"
						"X-Powered-By: OS HTTP Server\n"
						"Content-Type: text/html; charset=utf-8\n"
						"Content-Length: %d\n"
						"Date: Wed, %d\n"
						"Connection: close",
						strlen(html_not_found_response), time_string);

				char *response_string = malloc((strlen(header_html_not_found_response) + strlen(html_not_found_response) + 1) * sizeof(char));
				sprintf(response_string, "%s\n\n%s", header_html_not_found_response, html_not_found_response);
				// Enviar el mensaje de error
				nwritten = write(client_socketfd, response_string, strlen(response_string));
				free(response_string);
				if (nwritten < 0)
				{
					finished = 1;
					continue;
				}
			}
			// POST el archivo existe

			// En Caso contrario:
			// Encontrar el tamaño del archivo (stat)
			printf("Archivo encontrado\n");
			off_t file_size = stat_buffer.st_size;

			// Construir el encabezado
			
			time_t current_time;
			time(&current_time);
			char success_response[BUFSIZ];
			sprintf(success_response,
					"HTTP/1.1 200 OK\n"
					"X-Powered-By: OS HTTP Server\n"
					"Content-Type: %s\n"
					"Content-Length: %d\n"
					"Date: Wed, %d\n\n",
					get_extension(filename), file_size, ctime(&current_time));
			int headr_size = strlen(success_response);
			int remaining_space = BUFSIZ - headr_size - 1;
			success_response[BUFSIZ - 1] = '\0'; // El cliente recibe un primer mensaje en el cual hace fprintf

			// Enviar el encabezado seguido de \n\n
			// Abrir el archivo
			FILE *file_to_send = fopen(path, "r");
			if (file_to_send == NULL)
			{
				finished = 1;
				continue;
			}
			// leer el archivo en el final del header, la cantidad que lee es el espacio que queda en el buffer
			nread = fread(success_response + headr_size, sizeof(char), remaining_space, file_to_send);
			if (nread < 0)
			{
				finished = 1;
				continue;
			}
			// enviar el primer bloque
			nwritten = write(client_socketfd, success_response, BUFSIZ);
			if (nwritten < 0)
			{
				finished = 1;
				continue;
			}
			printf("written the first %d bytes in the client socket\n", nwritten);
			// Leer el archivo en bloques de BUFSIZ mientras haya datos
			//Si el tamaño del archivo es mayor al tamaño del buffer menos el tamaño del encabezado
			//enviar el archivo en bloques de BUFSIZ
			if (file_size > BUFSIZ - headr_size)
			{
				//mientras haya datos en el archivo por leer y no se haya terminado de enviar
				//el archivo al cliente enviar el archivo en bloques de BUFSIZ
				while ((nread = fread(success_response, sizeof(char), BUFSIZ, file_to_send)) > 0)
				{
					printf("read %d bytes from file\n", nread);
					//enviar el bloque leido al cliente y verificar que se haya enviado correctamente
					nwritten = write(client_socketfd, success_response, nread);
					//si no se envio correctamente terminar
					if (nwritten < 0)
					{
						finished = 1;
						continue;
					}
					printf("written %d bytes\n", nwritten);
				}
			}else{
				finished = 1;
			}
			fclose(file_to_send);
			// Cerrar la conexion y terminar el hilo
		}
	}

	// 5. Cerrar la conexion con el cliente
	close(client_socketfd);

	// 6. Cerrar el socket del servidor
	close(s);
	printf("Server finished.\n");

	exit(EXIT_SUCCESS);
}

void signal_handler(int num)
{
	printf("Signal %d received\n", num);
	finished = 1;
}
