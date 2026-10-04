NAME = binscope

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c17 -g

SRC = 	src/main.c \
		src/elf.c \
		src/sections.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -I include/

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re