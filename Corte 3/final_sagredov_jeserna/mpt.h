#ifndef MPT_H
#define MPT_H
/**
 * @brief Estructura de una entrada de la tabla de particiones
 * 
 */
typedef struct
{
    unsigned char boot_indicator;  /*!< 0x80 si es booteable, 0x00 si no*/
    unsigned char starting_CHS[3]; /*!< CHS de inicio de la particion*/
    unsigned char partition_type;  /*!< Tipo de particion*/
    unsigned char ending_CHS[3];   /*!< CHS de fin de la particion*/
    unsigned int starting_sector;  /*!< LBA de inicio de la particion*/
    unsigned int partition_size;   /*!< Tamaño de la particion en sectores*/
} PartitionEntry;

/**
 * @brief Estructura del MPT (Master partition table)
 * 
 */
#pragma pack(push, 1) // Empaquetar estructura para que no tenga padding
typedef struct
{
    PartitionEntry partition_table[4]; /*!< Tabla de particiones*/
    unsigned short signature;         /*!< 0xAA55 Boot record signature*/
} MPT;
#pragma pack(pop)
/**
 * @brief Imprime una entrada de la tabla de particiones
 * 
 * @param partition_entry  Entrada de la tabla de particiones
 */
void print_partition_entry(PartitionEntry *partition_entry);

/**
 * @brief Imprime la tabla de particiones
 * 
 * @param mpt Tabla de particiones
 */
void print_mpt(MPT *mpt);
#endif // MPT_H