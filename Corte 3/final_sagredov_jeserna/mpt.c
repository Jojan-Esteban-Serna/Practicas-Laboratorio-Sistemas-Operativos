#include "mpt.h"
#include <stdio.h>
#include "util.h"
#include "partition_names.h"

void print_partition_entry(PartitionEntry *partition_entry)
{
    // Imprimir el sector de inicio
    printf("%-16d", partition_entry->starting_sector);
    // Imprimir el sector de fin
    printf("%-16d", partition_entry->starting_sector + partition_entry->partition_size - 1);
    // Imprimir el tamaño de la particion
    print_size((long double)((long double)partition_entry->partition_size * 512));
    // Imprimir el codigo de particion
    char hexstring[8];
    unsigned char partition_type = partition_entry->partition_type;
    sprintf(hexstring, "%s%X00", partition_type < 0x10 ? "0" : "", partition_type);
    printf("%-16s", hexstring);
    // Imprimir el nombre de la particion
    initialize_partition_names();
    printf("%-16s\n", get_partition_name(partition_entry->partition_type));
}

void print_mpt(MPT *mpt)
{
    if (mpt->signature == 0xAA55)
    {
        printf("%-16s%-16s%-16s%-16s%-16s%-16s\n", "Number", "Start(sector)", "End (sector)", "Size", "Code", "Name");
        for (int i = 0; i < 4; i++)
        {
            if (mpt->partition_table[i].partition_type != 0x00)
            {
                // Imprimir el numero de particion
                printf("%s", mpt->partition_table[i].boot_indicator == 0x80 ? "*" : " ");
                printf("%-16d", i + 1);
                print_partition_entry(&mpt->partition_table[i]);
            }
        }
        printf("Signature: %X\n", mpt->signature);
    }
}