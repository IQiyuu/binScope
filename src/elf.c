#include "elf_parser.h"

char *getEndian(int n) {
    switch (n) {
    case 1:  return "Little-Endian";
    case 2:  return "Big-Endian";
    default: return "Unknown";
    }
}

char *getArchi(int n) {
    switch (n) {
    case 62:  return "x86_64";
    case 3:   return "x86";
    case 40:  return "ARM";
    case 183: return "AArch64";
    default:  return "Unknown";
    }
}

char *getType(int n) {
    switch (n) {
    case 1:  return "Object file";
    case 2:  return "Executable file";
    case 3:  return "Shared object file";
    case 4:  return "Core dump";
    default: return "Unknown type";
    }
}

char *getOsABI(int n) {
    switch (n) {
    case 0:  return "UNIX System V";
    case 1:  return "HP-UX";
    case 2:  return "NetBSD";
    case 3:  return "Linux";
    case 6:  return "Oracle Solaris";
    case 8:  return "Silicon Graphics IRIX";
    case 9:  return "FreeBSD";
    case 12: return "OpenBSD";
    case 64: return "ARM EABI";
    case 97: return "ARM";
    default: return "Unknown";
    }
}

int elf_read_header(int fd, Elf64_Ehdr *header)
{
        unsigned char ident[4];
    if (read_at(fd, ident, sizeof(ident), 0, "read_header"))
        return 1;
    if (memcmp(ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not an ELF file\n");
        return 1;
    }
    if (read_at(fd, header, sizeof(*header), 0, "read_header"))
        return 1;

    if (header->e_ident[0] != 0x7f || header->e_ident[1] != 'E' ||
        header->e_ident[2] != 'L'  || header->e_ident[3] != 'F')
    {
        fprintf(stderr, "Not an ELF file\n");
        return 1;
    }
    if (header->e_ident[4] != ELFCLASS64)
    {
        fprintf(stderr, "Only ELF64 is supported\n");
        return 1;
    }

    char magic[13];
    for (int i = 0; i < 4; i++)
        snprintf(magic + i * 3, 4, "%02X ", header->e_ident[i]);

    const char *class = "ELF64";
    const char *endian = getEndian(header->e_ident[5]);
    int version = header->e_ident[6];
    const char *machine = getArchi(header->e_machine);
    const char *type = getType(header->e_type);
    const char *osAbi = getOsABI(header->e_ident[7]);
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
        "ABI version: %d\n",
        magic, class, endian, version, machine, type, osAbi, abiVersion);

    printf("\n\n");

    printf(
        "Entry point:               0x%lx\n"
        "Program header offset:     0x%lx\n"
        "Section header offset:     0x%lx\n"
        "Number of program headers: %d\n",
        (unsigned long)header->e_entry, (unsigned long)header->e_phoff,
        (unsigned long)header->e_shoff, header->e_phnum);
    printf("\n");

    printf(
        "Number of sections:        %d\n"
        "Section header size:       %d\n",
        header->e_shnum, header->e_shentsize);
    printf("\n");

    printf(
        "ELF header size:           %d\n"
        "Flags:                     %u\n"
        "Section name string table: %d\n",
        header->e_ehsize, header->e_flags, header->e_shstrndx);

    return 0;
}