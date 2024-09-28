SRC = main.c ft_moves.c
OBJS= ${SRC:.c=.o}
NAME= cub3d
RM = rm -f
CC = cc -fsanitize=address -g3
CFLAGC = -Wall -Wextra -Werror

all: ${NAME}

LDFLAGS = -L/path/to/mlx/lib -lmlx -lXext -lX11 -lm

$(NAME): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) -Imlx -c $<

clean :
	${RM} ${OBJS}

fclean : clean
	${RM} ${NAME}

re: fclean all

.PHONY : all fclean clean re
