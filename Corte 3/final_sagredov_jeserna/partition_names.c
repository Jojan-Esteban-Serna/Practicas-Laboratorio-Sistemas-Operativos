#include "partition_names.h"
#include <string.h>
#include <stdio.h>
#define NUM_PARTITION_NAMES 0x100
const char *partition_names[NUM_PARTITION_NAMES];

void initialize_partition_names(){
    memset(partition_names, 0, sizeof(partition_names));

    partition_names[0x00] = "Empty";
    partition_names[0x01] = "FAT12";
    partition_names[0x02] = "XENIX root";
    partition_names[0x03] = "XENIX /usr";
    partition_names[0x04] = "FAT16 (<32MB)";
    partition_names[0x05] = "Extended";
    partition_names[0x06] = "FAT16 (>32MB)";
    partition_names[0x07] = "NTFS / HPFS";
    partition_names[0x08] = "AIX";
    partition_names[0x09] = "AIX bootable";
    partition_names[0x0B] = "FAT32";
    partition_names[0x0C] = "FAT32 (LBA)";
    partition_names[0x0E] = "FAT16 (LBA)";
    partition_names[0x0F] = "Extended (LBA)";
    partition_names[0x10] = "OPUS";
    partition_names[0x11] = "Hidden FAT12";
    partition_names[0x12] = "Compaq diagnostics";
    partition_names[0x14] = "Hidden FAT16 (<32MB)";
    partition_names[0x16] = "Hidden FAT16 (>32MB)";
    partition_names[0x17] = "Hidden NTFS / HPFS";
    partition_names[0x18] = "AST SmartSleep";
    partition_names[0x1B] = "Hidden FAT32";
    partition_names[0x1C] = "Hidden FAT32 (LBA)";
    partition_names[0x1E] = "Hidden FAT16 (LBA)";
    partition_names[0x24] = "NEC DOS";
    partition_names[0x27] = "Hidden NTFS WinRE";
    partition_names[0x39] = "Plan 9";
    partition_names[0x3C] = "PartitionMagic recovery";
    partition_names[0x40] = "Venix 80286";
    partition_names[0x41] = "PPC PReP Boot";
    partition_names[0x42] = "SFS";
    partition_names[0x4D] = "QNX4.x";
    partition_names[0x4E] = "QNX4.x 2nd part";
    partition_names[0x4F] = "QNX4.x 3rd part";
    partition_names[0x50] = "OnTrack DM";
    partition_names[0x51] = "OnTrack DM6 Aux";
    partition_names[0x52] = "CP/M";
    partition_names[0x53] = "OnTrack DM6 Aux";
    partition_names[0x54] = "OnTrackDM6";
    partition_names[0x55] = "EZ-Drive";
    partition_names[0x56] = "Golden Bow";
    partition_names[0x5C] = "Priam Edisk";
    partition_names[0x61] = "SpeedStor";
    partition_names[0x63] = "GNU HURD or SysV";
    partition_names[0x64] = "Novell Netware";
    partition_names[0x65] = "Novell Netware";
    partition_names[0x70] = "DiskSecure Multi-Boot";
    partition_names[0x75] = "PC/IX";
    partition_names[0x80] = "Old Minix";
    partition_names[0x81] = "Minix / old Linux";
    partition_names[0x82] = "Linux swap";
    partition_names[0x83] = "Linux";
    partition_names[0x84] = "OS/2 hidden C: drive";
    partition_names[0x85] = "Linux extended";
    partition_names[0x86] = "NTFS volume set";
    partition_names[0x87] = "NTFS volume set";
    partition_names[0x88] = "Linux plaintext";
    partition_names[0x8E] = "Linux LVM";
    partition_names[0x93] = "Amoeba";
    partition_names[0x94] = "Amoeba BBT";
    partition_names[0x9F] = "BSD/OS";
    partition_names[0xA0] = "IBM Thinkpad hibernation";
    partition_names[0xA1] = "HP-UX";
    partition_names[0xA3] = "HP-UX";
    partition_names[0xA4] = "HP-UX";
    partition_names[0xA5] = "FreeBSD";
    partition_names[0xA6] = "OpenBSD";
    partition_names[0xA7] = "NeXTSTEP";
    partition_names[0xA8] = "Darwin UFS";
    partition_names[0xA9] = "NetBSD";
    partition_names[0xAB] = "Darwin boot";
    partition_names[0xAF] = "HFS / HFS+";
    partition_names[0xB1] = "QNX Neutrino";
    partition_names[0xB3] = "QNX Neutrino";
    partition_names[0xB6] = "DOS access";
    partition_names[0xB7] = "BSDI fs";
    partition_names[0xB8] = "BSDI swap";
    partition_names[0xBB] = "Boot Wizard hidden";
    partition_names[0xBE] = "Solaris boot";
    partition_names[0xBF] = "Solaris";
    partition_names[0xC1] = "DRDOS/sec (FAT-12)";
    partition_names[0xC4] = "DRDOS/sec (FAT-16 < 32M)";
    partition_names[0xC6] = "DRDOS/sec (FAT-16)";
    partition_names[0xC7] = "Syrinx";
    partition_names[0xDA] = "Non-FS data";
    partition_names[0xDB] = "CP/M / CTOS / ...";
    partition_names[0xDE] = "Dell Utility";
    partition_names[0xDF] = "BootIt";
    partition_names[0xE1] = "DOS access";
    partition_names[0xE3] = "DOS R/O";
    partition_names[0xE4] = "SpeedStor";
    partition_names[0xEB] = "BeOS fs";
    partition_names[0xEE] = "GPT";
    partition_names[0xEF] = "EFI (FAT-12/16/32)";
    partition_names[0xF0] = "Linux/PA-RISC boot";
    partition_names[0xF1] = "SpeedStor";
    partition_names[0xF4] = "SpeedStor";
    partition_names[0xF2] = "DOS secondary";
    partition_names[0xFD] = "Linux RAID";
    partition_names[0xFE] = "LANstep";
    partition_names[0xFF] = "BBT";
}

const char *get_partition_name(int partition_type) {
    if (partition_type >= 0 && partition_type < NUM_PARTITION_NAMES) {
        const char* typeName = partition_names[partition_type];
        if (typeName != NULL) {
            return typeName;
        }
    }

    return "Unknown";
}