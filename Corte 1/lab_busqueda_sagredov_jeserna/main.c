/**
 * @file
 * @brief Busqueda de archivos
 * @author Santiago Agredo Vallejo   <sagredov@unicauca.edu.co>
 * @author Jojan Esteban Serna Serna <jeserna@unicauca.edu.co>
 */
#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

/**
 * @brief Muestra la ayuda del programa
 * @param[in] program_name Nombre del programa
 */
void usage(char const *program_name);

/**
 * @brief Funcion para verificar si una ruta dada es un directorio
 * @param[in] ruta Ruta para la cual se quiere verificar si es un directorio o no
 * @return Retorna 1 si la ruta dada es un directorio, 0 en caso contrario
 */
int es_directorio(char const *ruta);

/**
 * @brief Busca recursivamente archivos/directorios cuyo nombre contenga patron
 * @param[in] directorio Directorio sobre el cual se esta buscando el patron
 * @param[in] patron Patron a buscar
 * @return Retorna el total de coincidencias
 */
int buscar(char const *directorio, char const *patron);

int main(int argc, char const *argv[])
{
    const char *dir;
    const char *pattern;
    // Terminar temprano
    if (argc != 3)
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    // PRE: argc == 3

    dir = argv[1];
    pattern = argv[2];
    buscar(dir, pattern);
    // TODO
    exit(EXIT_SUCCESS);
}

void usage(char const *program_name)
{
    printf("Busqueda de archivos\n");
    printf("Uso: %s DIR PATTERN\n", program_name);
    printf("\tDIR: Directorio base de busqueda\n");
    printf("\tPATTERN: Texto contenido en el nombre del archivo/directorio\n");
    printf("Busca los archivo(s) y/o directorio(s) dentro de DIR cuyo nombre\ncontiene PATTERN\n");
}

int es_directorio(char const *ruta)
{
    // Usar el servicio stat para obtener la información de la ruta dada
    struct stat s;
    int resultado = stat(ruta, &s);
    if (resultado < 0) // error
    {
        perror("stat");
        return 0; // C no tiene tipo booleano, 0 = falso
    }
    // Usar el macro S_ISDIR sobre el atributo st_mode de la estructura
    // para verificar si es un directorio
    if (S_ISDIR(s.st_mode))
    {
        return 1; // C no tiene tipo booleano, 1 = verdadero
    }
    else
    {
        return 0;
    }
}

int buscar(char const *directorio, char const *patron)
{
    int total = 0;
    // Abrir el directorio con opendir
    DIR *d = opendir(directorio);
    // Validar el resultado de la llamada al servicio opendir
    // Tenga en cuenta! la comparación de igualdad en C es con ==
    if (d == NULL)
    {
        perror("opendir");
        return total;
    }
    // Leer el directorio con el servicio readdir
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL)
    {
        // Construir la ruta completa directorio/ent->d_name
        char *sep = "/";
        size_t tam = strlen(directorio) + strlen(sep) + strlen(ent->d_name) + 1;
        char *ruta = (char *)malloc(tam * sizeof(char));
        sprintf(ruta, "%s%s%s", directorio, sep, ent->d_name);

        // Tenga en cuenta! En C no se puede concatenar las cadenas de
        // esta forma
        // Se debe reservar memoria suficiente con malloc, usar strcpy
        // para copiar y strcat para concatenar.
        // La cadena de destino (ruta) debe tener suficiente espacio
        // para almacenar directorio, "/" y ent->d_name.
        // Toda cadena válida en C termina en nulo, por lo tanto se
        // debe reservar 1 byte más de memoria

        // Verificar si la entrada (archivo o directorio) contiene
        // el patrón de búsqueda
        if (strstr(ent->d_name, patron))
        {
            // imprimir la ruta completa
            // Reservar memoria para la ruta completa
            char ruta_completa[PATH_MAX];

            // Obtener la ruta ruta completa y verificar que realpath no devuelva NULL
            if (realpath(ruta, ruta_completa) == NULL)
            {
                perror("realpath");
                return EXIT_FAILURE;
            }
            printf("%s\n", ruta_completa);
            total++;
        }
        // Si la entrada es un directorio, se debe buscar
        // recursivamente!
        // Precaucion: descartar los directorios "." (actual)
        // y ".." (anterior) para prevenir que la funcion nunca termine
        // Tenga en cuenta! En C, las cadenas se comparan como strcmp
        if (es_directorio(ruta) && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
        {
            // Buscar recursivamente dentro del subdirectorio
            total += buscar(ruta, patron);
        }
        free(ruta);
    }
    // Cerrar el directorio
    closedir(d);
    return total;
}