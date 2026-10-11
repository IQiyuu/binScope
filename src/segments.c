#define _POSIX_C_SOURCE 200809L
#include "elf_parser.h"

static const char *getSegType(Elf64_Word t) {
    switch (t) {
        case PT_NULL:         return "NULL";
        case PT_LOAD:         return "LOAD";
        case PT_DYNAMIC:      return "DYNAMIC";
        case PT_INTERP:       return "INTERP";
        case PT_NOTE:         return "NOTE";
        case PT_SHLIB:        return "SHLIB";
        case PT_PHDR:         return "PHDR";
        case PT_TLS:          return "TLS";
        case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
        case PT_GNU_STACK:    return "GNU_STACK";
        case PT_GNU_RELRO:    return "GNU_RELRO";
        default:              return "OTHER";
    }
}

static void getSegFlags(char out[4], Elf64_Word f) {
    out[0] = (f & PF_R) ? 'R' : ' ';
    out[1] = (f & PF_W) ? 'W' : ' ';
    out[2] = (f & PF_X) ? 'X' : ' ';
    out[3] = 0;
}

int elf_read_segments(int fd, Elf64_Ehdr *header) {
    if (header->e_phnum == 0 || header->e_phoff == 0) {
        printf("No program headers\n");
        return 0;
    }
    if (header->e_phentsize != sizeof(Elf64_Phdr)) {
        fprintf(stderr, "elf_read_segments: invalid phentsize\n");
        return 1;
    }

    printf("\n\n%-4s %-14s %-10s %-18s %-8s %-8s %-5s %s\n",
           "[Nr]", "type", "offset", "vaddr", "filesz", "memsz", "flags", "align");
    for (int i = 0; i < header->e_phnum; i++) {
        Elf64_Phdr ph;
        char flags[4];
        off_t off = (off_t)header->e_phoff + (off_t)header->e_phentsize * i;

        if (read_at(fd, &ph, sizeof(ph), off, "read_segment"))
            return 1;
        getSegFlags(flags, ph.p_flags);
        printf("[%2d] %-14s 0x%06lx   0x%016lx 0x%06lx 0x%06lx %-5s 0x%lx\n",
               i, getSegType(ph.p_type),
               (unsigned long)ph.p_offset, (unsigned long)ph.p_vaddr,
               (unsigned long)ph.p_filesz, (unsigned long)ph.p_memsz,
               flags, (unsigned long)ph.p_align);
    }
    return 0;
}