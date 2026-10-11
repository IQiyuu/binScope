    #ifndef ELF_H
    #define ELF_H

    #include <elf.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    int read_at(int, void *, size_t, off_t, const char *);

    int elf_read_header(int, Elf64_Ehdr *);
    int elf_read_sections(int, Elf64_Ehdr *);
    int elf_read_symbols(int, Elf64_Ehdr *, Elf64_Shdr *, const char *);
    int elf_read_segments(int, Elf64_Ehdr *);

    char *load_strtab(int, Elf64_Ehdr *, int, size_t *);
    char *load_tabs(int, Elf64_Ehdr *, int);

    #endif