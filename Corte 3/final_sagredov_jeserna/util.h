#ifndef UTIL_H
#define UTIL_H
/**
 * @brief Lee un archivo y lo guarda en un buffer
 * 
 * @param filename Nombre del archivo
 * @param struct_ptr  Puntero a la estructura donde se guardara el archivo
 * @param struct_size  Tamaño de la estructura
 * @param offset  Offset desde donde se empieza a leer el archivo
 * @return int  1 si se pudo leer el archivo, 0 si no
 */
int read_struct_from_file(char *filename, void *struct_ptr, int struct_size, int offset);
/**
 * @brief Imprime un tamaño dado en Bytes, KB, MB o GB
 * 
 * @param size  Tamaño a imprimir (debe estar casteado a long double y ese numero casteado debes ser long)
 */
void print_size(long double size);

#endif // UTIL_H