#define _POSIX_C_SOURCE 200809L
#include "elf_parser.h"

char *load_strtab(int fd, Elf64_Ehdr *header, int index, size_t *size) {
    if (index <= 0 || index >= header->e_shnum) {
        fprintf(stderr, "load_tabs: invalid section index %d\n", index);
        return NULL;
    }
    Elf64_Shdr sh;
    off_t hoff = (off_t)header->e_shoff + (off_t)header->e_shentsize * index;
    if (read_at(fd, &sh, sizeof(sh), hoff, "load_tabs read header")) {
        return NULL;
    }
    if (sh.sh_size == (Elf64_Xword)-1) {
        fprintf(stderr, "load_tabs: invalid section size\n");
        return NULL;
    }
    char *buf = malloc(sh.sh_size + 1);
    if (!buf) {
        perror("load_tabs malloc");
        return NULL;
    }
    if (read_at(fd, buf, sh.sh_size, sh.sh_offset, "load_tabs read content")) {
        free(buf);
        return NULL;
    }
    buf[sh.sh_size] = 0;
    if (size)
        *size = sh.sh_size;
    return buf;
}

char *load_tabs(int fd, Elf64_Ehdr *header, int index) {
    return load_strtab(fd, header, index, NULL);
}

void getFlags(char flags[16], Elf64_Xword value) {
    int i = 0;
    if (value & SHF_WRITE) flags[i++] = 'W';
    if (value & SHF_ALLOC) flags[i++] = 'A';
    if (value & SHF_EXECINSTR) flags[i++] = 'X';
    flags[i] = 0;
}

static const char *getSectionType(Elf64_Word t) {
    switch (t) {
    case SHT_NULL:          return "NULL";
    case SHT_PROGBITS:      return "PROGBITS";
    case SHT_SYMTAB:        return "SYMTAB";
    case SHT_STRTAB:        return "STRTAB";
    case SHT_RELA:          return "RELA";
    case SHT_HASH:          return "HASH";
    case SHT_DYNAMIC:       return "DYNAMIC";
    case SHT_NOTE:          return "NOTE";
    case SHT_NOBITS:        return "NOBITS";
    case SHT_REL:           return "REL";
    case SHT_DYNSYM:        return "DYNSYM";
    case SHT_INIT_ARRAY:    return "INIT_ARRAY";
    case SHT_FINI_ARRAY:    return "FINI_ARRAY";
    case SHT_PREINIT_ARRAY: return "PREINIT_ARRAY";
    case SHT_GROUP:         return "GROUP";
    default:                return "OTHER";
    }
}

int read_at(int fd, void *buf, size_t size, off_t off, const char *what) {
    ssize_t n = pread(fd, buf, size, off);
    if (n < 0) {
        perror(what);
        return 1;
    }
    if ((size_t)n != size) {
        fprintf(stderr, "%s: truncated file\n", what);
        return 1;
    }
    return 0;
}   

int elf_read_sections(int fd, Elf64_Ehdr *header) {
    size_t names_size = 0;
    char *names = load_strtab(fd, header, header->e_shstrndx, &names_size);
    if (!names)
        return 1;

    printf("\n\n");
    Elf64_Shdr symtab_sh = {0};
    Elf64_Shdr dynsym_sh = {0};

    printf("%-4s %-20s %-14s %-10s %-8s %-8s %-6s %-4s\n",
           "[Nr]", "name", "type", "addr", "offset", "size", "flags", "link");
    for (int i = 0; i < header->e_shnum; i++) {
        Elf64_Shdr sh;
        char flags[16];
        off_t hoff = (off_t)header->e_shoff + (off_t)header->e_shentsize * i;
        if (read_at(fd, &sh, sizeof(sh), hoff, "read_section")) {
            free(names);
            return 1;
        }
        const char *name = "<invalid>";
        if (sh.sh_name < names_size)
            name = names + sh.sh_name;
        getFlags(flags, sh.sh_flags);
        if (!strcmp(name, ".symtab")) symtab_sh = sh;
        if (sh.sh_type == SHT_DYNSYM) dynsym_sh = sh;
        printf("[%2d] %-20s %-14s 0x%08lx 0x%06lx 0x%06lx %-6s %u\n",
               i, name, getSectionType(sh.sh_type),
               (unsigned long)sh.sh_addr, (unsigned long)sh.sh_offset,
               (unsigned long)sh.sh_size, flags, sh.sh_link);
    }
     free(names);
    int ret = elf_read_symbols(fd, header, &symtab_sh, ".symtab");
    ret |= elf_read_symbols(fd, header, &dynsym_sh, ".dynsym");
    return ret;
}