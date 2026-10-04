    #ifndef ELF_H
    #define ELF_H

    #include <elf.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    int elf_read_header(int, Elf64_Ehdr *);
    int elf_read_sections(int, Elf64_Ehdr *);
    char *load_tabs(int, Elf64_Ehdr *, int);

    #endif