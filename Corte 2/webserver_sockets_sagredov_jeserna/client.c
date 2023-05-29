
/**
 * @file
 * @brief Servidor
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 */

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#ifndef PATH_MAX
#include <linux/limits.h>
#endif
#include "util.h"

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

	// Socket del servidor
	int server_socketfd;

	// Conexion del cliente
	int client_socketfd;

	// Direccion IPv4 del servidor
	struct sockaddr_in addr;

	// Estructura de accion de señal
	struct sigaction act;

	// Buffer de lectura de la entrada estandar
	char inbuf[BUFSIZ];
	// Buffer para escribir datos en el socket del servidor

	char server_write_buf[BUFSIZ];
	// Buffer para leer datos desde el socket del servidor

	char server_read_buf[BUFSIZ];

	ssize_t nread;	  // Count of bytes read.
	ssize_t nwritten; // count of bytes written.

	// Inicializar la estructura con ceros (NULL)
	memset(&act, 0, sizeof(struct sigaction));

	// Establecer el manejador de la señal
	act.sa_handler = signal_handler;

	// Instalar el manejador de la señal SIGINT
	sigaction(SIGINT, &act, NULL);

	// Instalar el manejador de la señal SIGTERM
	sigaction(SIGTERM, &act, NULL);

	// 1. Crear un socket IPv4, flujo (stream)
	server_socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socketfd < 0)
	{
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	// Inicializar la direccion antes de conectarse
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234); // TODO argumento de linea de comandos!!!
	if (inet_aton("127.0.0.1", &addr.sin_addr) != 1)
	{ // Conectarse a la direction TODO del servidor
		fprintf(stderr, "Unable to parse the address\n");
		exit(EXIT_FAILURE);
	}

	// 2. Conectarse a la direccion addr de tipo IPv4  (bind)
	if (connect(server_socketfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	// POST: Connection established.
	printf("Client connected.\n");

	while (!finished)
	{

		// Leer el nombre del archivo de la entrada estandar
		printf("Enter the file name: ");
		scanf("%s", inbuf);
		// Construir y enviar el mensaje GET /ruta_archivo HTTP/1.1\n\n
		sprintf(server_write_buf, "GET /%s HTTP/1.1\n\n", inbuf);
		nwritten = write(server_socketfd, server_write_buf, strlen(server_write_buf));
		if (nwritten < 0)
		{
			finished = 1;
			continue;
		}
		// Leer la respuesta
		nread = read(server_socketfd, server_read_buf, BUFSIZ);
		if (nread < 0)
		{
			finished = 1;
			continue;
		}
		// Si la respuesta es 404 Not Found:
		if (strstr(server_read_buf, "404 Not Found") != NULL)
		{
			printf("%s", server_read_buf);
			finished = 1;
			continue;
		}
		// Si la respuesta es 200 OK:
		if (strstr(server_read_buf, "200 OK") != NULL)
		{
			// Leer el encabezado del mensaje (hasta encontrar \n\n)
			char *end_header = strstr(server_read_buf, "\n\n");
			// Despues del encabezado se recibe el contenido del archivo
			// Leer el contenido del archivo y guardar dentro del directorio files/
			// Apuntar a donde inicia el archivo
			char *file_content = end_header + 2;
			// Crear la ruta del archivo
			char file_path[PATH_MAX];
			// crearla (inbuf tenia lo ingresado por teclado)
			sprintf(file_path, "files/%s", inbuf);
			printf("Saving file %s\n", file_path);
			//Se crea el directorio files
			char * folder_name = "files";
			//Si no existe el directorio files se crea
			if(access(folder_name, F_OK) == -1){
				//Se crea el directorio files
				mkdir(folder_name, 0700);
				printf("Created folder\n");
			}
			printf("Checked folder\n");
			// abrir el archivo (se crea si no existe), se abre en modo append
			FILE *file = fopen(file_path, "a+");
			if (file == NULL)
			{
				perror("fopen");
				continue;
			}
			printf("Opened file\n");
			// Escribir el primer bloque del archivo desde donde termina el encabezado
			fprintf(file, "%s", file_content);
			printf("Saved the first block\n");
			// leer el resto del archivo del servidor y escribirlo en el archivo local
			while ((nread = read(server_socketfd, server_read_buf, BUFSIZ) )> 0)
			{
				printf("Readed %d bytes from the server socket\n", nread);
				// escribir el contenido del archivo se agregara al final
				nwritten = fwrite(server_read_buf, 1, nread, file);
				if(nwritten < 0){
					finished = 1;
					continue;
				}
				printf("Writed %d bytes to the file\n", nwritten);
			}

			fclose(file);
			printf("File %s saved.\n", file_path);
			finished = 1;
		}
	}

	// 3. Cerrar la conexion con el servidor
	close(server_socketfd);
	printf("Client finished.\n");

	exit(EXIT_SUCCESS);
}

void signal_handler(int num)
{
	printf("Signal %d received\n", num);
	finished = 1;
}
