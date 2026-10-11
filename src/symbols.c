#define _POSIX_C_SOURCE 200809L
#include "elf_parser.h"

static const char *getSymType(unsigned char t) {
    switch (t) {
        case STT_NOTYPE:  return "NOTYPE";
        case STT_OBJECT:  return "OBJECT";
        case STT_FUNC:    return "FUNC";
        case STT_SECTION: return "SECTION";
        case STT_FILE:    return "FILE";
        case STT_COMMON:  return "COMMON";
        case STT_TLS:     return "TLS";
        default:          return "OTHER";
    }
}

static const char *getSymBind(unsigned char b) {
    switch (b) {
        case STB_LOCAL:  return "LOCAL";
        case STB_GLOBAL: return "GLOBAL";
        case STB_WEAK:   return "WEAK";
        default:         return "OTHER";
    }
}

static void getShndx(char out[8], Elf64_Half shndx) {
    switch (shndx) {
        case SHN_UNDEF:  snprintf(out, 8, "UND"); break;
        case SHN_ABS:    snprintf(out, 8, "ABS"); break;
        case SHN_COMMON: snprintf(out, 8, "COM"); break;
        default:         snprintf(out, 8, "%u", shndx);
    }
}

int elf_read_symbols(int fd, Elf64_Ehdr *header, Elf64_Shdr *symtab_sh,
    const char *title) {
    if (symtab_sh->sh_type != SHT_SYMTAB && symtab_sh->sh_type != SHT_DYNSYM) {
        printf("\n\nNo %s\n", title);
        return 0;
    }
    if (symtab_sh->sh_entsize != sizeof(Elf64_Sym)) {
        fprintf(stderr, "elf_read_symbols: invalid entry size\n");
        return 1;
    }

    size_t strtab_size = 0;
    char *strtab = load_strtab(fd, header, symtab_sh->sh_link, &strtab_size);
    if (!strtab)
        return 1;

    size_t count = symtab_sh->sh_size / sizeof(Elf64_Sym);

    printf("\n\n%s\n", title);
    printf("%-6s %-18s %-6s %-8s %-8s %-6s %s\n",
           "[Nr]", "value", "size", "type", "bind", "shndx", "name");
        for (size_t i = 0; i < count; i++) {
        Elf64_Sym sym;
        char shndx[8];

        off_t off = (off_t)symtab_sh->sh_offset + (off_t)(i * sizeof(sym));
        if (read_at(fd, &sym, sizeof(sym), off, "read_symbol")) {
            free(strtab);
            return 1;
        }
        getShndx(shndx, sym.st_shndx);

        const char *name = "<invalid>";
        if (sym.st_name < strtab_size)
            name = strtab + sym.st_name;
        printf("[%4zu] 0x%016lx %-6lu %-8s %-8s %-6s %s\n",
               i, (unsigned long)sym.st_value, (unsigned long)sym.st_size,
               getSymType(ELF64_ST_TYPE(sym.st_info)),
               getSymBind(ELF64_ST_BIND(sym.st_info)),
               shndx, name);
    }
    free(strtab);
    return 0;
}