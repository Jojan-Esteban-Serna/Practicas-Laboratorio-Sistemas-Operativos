#ifndef PARTITION_NAMES_H
#define PARTITION_NAMES_H


/**
 * @brief Inicializa el arreglo de nombres de particiones.
 *
 */
void initialize_partition_names();

/**
 * @brief Obtiene el nombre de una partición.
 *
 * @param partition_number  Número de partición.
 * @return const char*    Nombre de la partición.
 */
const char *get_partition_name(int partition_number);

#endif // PARTITION_NAMES_H