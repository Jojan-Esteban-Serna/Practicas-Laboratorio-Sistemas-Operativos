/**
 * @file main.c
 * @author Jojan Esteban Serna Serna (jeserna@unicauca.edu.co)
 * @brief
 * @version
 * @date 2023-03-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "split.h"

#define EQUALS(s1, s2) (strcmp(s1, s2) == 0)

int main(int argc, char *argv[])
{
	bool finished;
	finished = false;
	int len;
	split_list *list;
	pid_t pid;
	int return_status;

	char linea[BUFSIZ];
	while (!finished)
	{
		printf("$ ");

		// Limpiar la linea
		memset(linea, BUFSIZ, 0);
		// leer una linea
		if (fgets(linea, BUFSIZ, stdin) == NULL)
		{
			// Repetir la lectura de la linea

			continue;
		}
		// POST: linea leida
		len = strlen(linea);

		if (len <= 1)
		{
			// Repetir la lectura de la linea
			continue;
		}

		// POST: La linea contiene al menos un caracter
		// Quitar el NEWLINE al final

		if (linea[len - 1] == '\n')
		{
			linea[len - 1] = 0;
			len--;
		}

		if (linea[0] == '#')
		{
			// Repetir la lectura de la linea

			continue;
		}

		// Partir la linea

		list = split(linea, " \t\n|");

		if (EQUALS(list->parts[0], "exit") || EQUALS(list->parts[0], "quit"))
		{
			// Marcar la bandera d eterminacion y repetir el ciclo
			finished = true;
			continue;
		}

		pid = fork();
		if (pid > 0)
		{
			// Proceso padre: Esperar que el hijo termine
			waitpid(pid, 0, 0);
		}
		else
		{ 
			// Copia: Ejecuta rel comando que se encuentra enn list->parts
			execvp(list->parts[0], list->parts);
			//Si execvp retorna, el comando no se puede ejecutar
			//Imprimir el mensaje de error de la llamada al sistema execvp
			perror(list->parts[0]);
			//Terminar inmediatamente
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}
