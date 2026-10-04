#include "elf_parser.h"

char *load_tabs(int fd, Elf64_Ehdr *header, int index) {
    if (index <= 0 || index >= header->e_shnum) {
        fprintf(stderr, "load_tabs: invalid section index %d\n", index);
        return NULL;
    }
    if (lseek(fd, header->e_shoff + (off_t)header->e_shentsize * index, SEEK_SET) == -1) {
        perror("load_tabs lseek header");
        return NULL;
    }
    Elf64_Shdr sh;
    if (read(fd, &sh, sizeof(sh)) != sizeof(sh)) {
        perror("load_tabs read header");
        return NULL;
    }
    char *buf = malloc(sh.sh_size + 1);
    if (!buf) {
        perror("load_tabs malloc");
        return NULL;
    }
    if (lseek(fd, sh.sh_offset, SEEK_SET) == -1) {
        perror("load_tabs lseek content");
        free(buf);
        return NULL;
    }
    if (read(fd, buf, sh.sh_size) != (ssize_t)sh.sh_size) {
        perror("load_tabs read content");
        free(buf);
        return NULL;
    }
    buf[sh.sh_size] = 0;
    return buf;
}


void getFlags(char flags[16], Elf64_Xword value) {
    int i = 0;
    if (value & SHF_WRITE) flags[i++] = 'W';
    if (value & SHF_ALLOC) flags[i++] = 'A';
    if (value & SHF_EXECINSTR) flags[i++] = 'X';
    flags[i] = 0;
}

int elf_read_sections(int fd, Elf64_Ehdr *header) {
    char *names = load_tabs(fd, header, header->e_shstrndx);
    if (!names)
        return 1;
    if (lseek(fd, header->e_shoff, SEEK_SET) == -1) {
        perror("shstrab lseek");
        free(names);
        return 1;
    }

    printf("\n\n");
    Elf64_Shdr symtab_sh = {0};

    printf("%-4s %-12s %-4s %-10s %-8s %-8s %-8s %-8s\n",
        "[Nr]", "name", "type", "addr", "offset", "size", "flags", "link");
    for (int i = 0; i < header->e_shnum; i++) {
        Elf64_Shdr sh;
        char flags[16];
        if (read(fd, &sh, sizeof(sh)) != sizeof(sh)) {
            perror("read_section");
            free(names);
            return 1;
        }
        getFlags(flags, sh.sh_flags);
        if (!strcmp(names + sh.sh_name, ".symtab")) symtab_sh = sh;
        printf("[%2d] %-12s %-4u 0x%08lx 0x%06lx 0x%06lx %-8s %u\n",
            i, names + sh.sh_name, sh.sh_type,
            sh.sh_addr, sh.sh_offset, sh.sh_size, flags, sh.sh_link);
    }
    free(names);
    (void)symtab_sh;
    // elf_read_symbols(fd, header, symtab_sh)
    return 0;
}