#include "elf_parser.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <binary>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    Elf64_Ehdr header;

    elf_read_header(fd, &header);

    elf_read_sections(fd, &header);
    
    close(fd);
}