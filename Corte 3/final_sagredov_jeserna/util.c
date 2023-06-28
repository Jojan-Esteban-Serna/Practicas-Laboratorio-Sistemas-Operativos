#include <stdio.h>
#include <stdlib.h>
#include "util.h"
int read_struct_from_file(char *filename, void *struct_ptr, int struct_size, int offset)
{
    // Abrir el archivo
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error: No se encontro el archivo\n");
        return 0;
    }
    // Obtener el tamaño del archivo
    fseek(file, offset, SEEK_END);
    long file_size = ftell(file);

    // Apuntar al offset especificado desde el inicio del archivo
    fseek(file, offset, SEEK_SET);

    // Verificar que el archivo sea lo suficientemente grande
    if (file_size < offset + (long)struct_size)
    {
        printf("Error: El archivo especificado es muy pequeño\n");
        fclose(file);
        return 0;
    }
    // Leer la estructura
    if (fread(struct_ptr, struct_size, 1, file) != 1)
    {
        printf("Error: No se pudo leer la estructura del archivo\n");
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}

void print_size(long double size)
{
    const char *units[] = {"bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB", "RiB", "QiB"};
    int index = 0;

    while (size >= 1024 && index < 10)
    {
        size /= 1024;
        index++;
    }
    char formatted[20];
    sprintf(formatted, "%.2Lf %s", size, units[index]);
    printf("%-16s", formatted);
}

int chstolba(int c, int h, int s, int nh, int ns)
{
    c = ((0b11000000 & s) << 2) | c;
    s = 0b00111111 & s;
    return (c * nh * ns) + (h * ns) + (s - 1);
}