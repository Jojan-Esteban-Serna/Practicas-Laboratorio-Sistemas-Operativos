#include "mpt.h"
#include <stdio.h>
#include "util.h"
#include "partition_names.h"

void print_mpt(MPT *mpt)
{

    printf("%-16s%-16s%-16s%-16s%-16s%-16s%-16s%-16s%-16s%-16s\n", "Number", "Start(sector)", "End (sector)", "Size", "Code", "Name","CHS Start(SPT=63)", "CHS Start(SPT=2)","CHS End(SPT=63)", "CHS End(SPT=2)");
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
}

void print_partition_entry(PartitionEntry *partition_entry)
{
    // Imprimir el sector de inicio
    long double start_sec = print_start(partition_entry);
    // Imprimir el sector de fin
    long double end_sec = print_end(partition_entry);
    // Imprimir el tamaño de la particion
    print_size((long double)(end_sec - start_sec + 1) * 512);
    // Imprimir el codigo de particion
    char hexstring[8];
    unsigned char partition_type = partition_entry->partition_type;
    sprintf(hexstring, "%s%X00", partition_type < 0x10 ? "0" : "", partition_type);
    printf("%-16s", hexstring);
    // Imprimir el nombre de la particion
    initialize_partition_names();
    printf("%-16s", get_partition_name(partition_entry->partition_type));
    int chsstart63 = chstolba(partition_entry->starting_CHS[2], partition_entry->starting_CHS[0], partition_entry->starting_CHS[1], 255, 63);
    int chsstart2 = chstolba(partition_entry->starting_CHS[2], partition_entry->starting_CHS[0] ,partition_entry->starting_CHS[1], 255, 2);

    int chsend63 = chstolba(partition_entry->ending_CHS[2], partition_entry->ending_CHS[0] ,partition_entry->ending_CHS[1], 255, 63);
    int chsend2 = chstolba(partition_entry->ending_CHS[2], partition_entry->ending_CHS[0] ,partition_entry->ending_CHS[1], 255, 2);

    printf("%-16d", chsstart63);
    printf("%-16d", chsstart2);
    printf("%-16d", chsend63);
    printf("%-16d\n", chsend2);
}

unsigned int print_start(PartitionEntry *partition_entry)
{
    if (partition_entry->starting_sector != 0 && partition_entry->partition_size != 0)
    {
        printf("%-16d", partition_entry->starting_sector);
        return partition_entry->starting_sector;
    }
}
unsigned int print_end(PartitionEntry *partition_entry)
{
    if (partition_entry->starting_sector != 0 && partition_entry->partition_size != 0)
    {
        printf("%-16d", partition_entry->starting_sector + partition_entry->partition_size - 1);
        return partition_entry->starting_sector + partition_entry->partition_size - 1;
    }
}
