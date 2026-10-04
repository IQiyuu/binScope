#include "elf_parser.h"

char *intToHex(int n) {
    char *res = malloc(3);

    res[0] = n / 16 + ((n / 16 < 10) ? '0' : 'A'-10);
    res[1] = n % 16 + ((n % 16 < 10) ? '0' : 'A'-10);
    res[2] = 0;
    return res;
}

char *getEndian(int n) {
    char *endian;
    switch (n) {
        case 0:
            endian = "Unknow type";
            break ;
        case 1:
            endian = "Little-Endian";
            break ;
        default:
            endian = "Big-Endian";
    }
    return endian;
}

char *getArchi(int n) {
    char *archi;
    switch (n) {
        case 62:
            archi = "x86_64";
            break ;
        case 3:
            archi = "x86";
            break ;
        case 40:
            archi = "ARM";
            break ;
        case 183:
            archi = "AArch64";
            break ;
        default:
            archi = "Unknow";
    }
    return archi;
}

char *getType(int n) {
    char *type;
    switch (n) {
        case 1:
            type = "Object file";
            break ;
        case 2:
            type = "Executable file";
            break ;
        case 3:
            type = "Shared object file";
            break ;
        case 4:
            type = "Core dump";
            break ;
        default:
            type = "Unknow type";
    }
    return type;
}

char *getOsABI(int n) {
    char *osABI;
    switch (n) {
        case 0:
            osABI = "UNIX System V";
            break ;
        case 1:
            osABI = "HP-UX";
            break ;
        case 2:
            osABI = "NetBSD";
            break ;
        case 3:
            osABI = "Linux";
            break ;
        case 6:
            osABI = "Oracle Solaris";
            break ;
        case 8:
            osABI = "Silicon Graphics IRIX";
            break ;
        case 9:
            osABI = "FreeBSD";
            break ;
        case 12:
            osABI = "OpenBSD";
            break ;
        case 64:
            osABI = "ARM EABI";
            break ;
        case 97:
            osABI = "ARM";
            break ;
    }
    return osABI;
}


int elf_read_header(int fd, Elf64_Ehdr *header)
{
    if (read(fd, header, sizeof(*header)) != sizeof(*header))
    {
        perror("read_parser");
        close(fd);
        return 1;
    }

    char *magic = malloc(13);
    memset(magic, 0, 13);
    for (int i = 0; i < 4; i++) {
        char *tmp = intToHex(header->e_ident[i]);
        strcat(magic, tmp);
        strcat(magic, " ");
        free(tmp);
    }

    char *class = (header->e_ident[4] == 1 ? "ELF32" : "ELF64");

    char *endian = getEndian(header->e_ident[5]);
    
    int version = header->e_ident[6];

    char *machine = getArchi(header->e_machine);

    char *type = getType(header->e_type);

    char *osAbi = getOsABI(header->e_ident[7]);

    int abiVersion = header->e_ident[8];

    printf("\n\n");

    printf(
        "ELF Header\n"
        "──────────\n"
        "Magic:       %s\n"
        "Class:       %s\n"
        "Endianness:  %s\n"
        "Version:     %d\n"
        "Machine:     %s\n"
        "Type:        %s\n"
        "OS/ABI:      %s\n"
        "ABI version: %d\n"
    , magic, class, endian, version, machine, type, osAbi, abiVersion);

    printf("\n\n");

    printf(
        "Entry point:               0x%lx\n"
        "Program header offset:     0x%lx\n"
        "Section header offset:     0x%lx\n"
        "Number of program headers: %x\n"
        , header->e_entry, header->e_phoff, header->e_shoff, header->e_phnum);
    
    printf("\n");

    printf(
        "Number of sections:        %d\n"
        "Section header size:       %d\n"
        , header->e_shnum, header->e_shentsize);
    
    printf("\n");

    printf(
        "ELF header size:           %d\n"
        "Flags:                     %d\n"
        "Section name string table: %d\n"
    , header->e_ehsize, header->e_flags, header->e_shstrndx);

    
    
    free(magic);

    return 0;
}