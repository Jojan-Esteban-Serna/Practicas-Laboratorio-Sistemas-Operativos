#include <stdio.h>
#include <stdlib.h>
#include "mpt.h"
#include "util.h"
/**
 * @brief Imprime el uso correcto del programa
 *
 * @param program_name  Nombre del programa
 */
void usage(char *program_name);

int main(int argc, char *argv[])
{
	if (argc > 2 || argc < 2)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// TODO Imprimir la tabla de particiones!!

	// La tabla de particones se encuentra a un desplazamiento de 446 bytes
	// a partir del inicio del sector de arranque y ocupa exactamente 64 bytes

	// Despues de la tabla de particiones, se encuentran los bytes 0xAA55.

	MPT mpt;
	// Leer la tabla de particiones a un offset de 446 bytes en el archivo pasado por linea de comandos
	int result = read_struct_from_file(argv[1], &mpt, sizeof(MPT), 446);
	if (result == 0)
	{
		printf("Error al cargar el struct desde el archivo\n");
		exit(EXIT_FAILURE);
	}
	if(mpt.signatur != 0xAA55)
	{
		printf("Error: No se encontro la firma 0xAA55\n");
		exit(EXIT_FAILURE);
	}
	print_mpt(&mpt);

	exit(EXIT_SUCCESS);
}

void usage(char *program_name)
{
	printf("Usage:\n");
	printf("%s <filename>\n", program_name);
	exit(EXIT_FAILURE);
}

