NAME    = ft_ping
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
SRCS    = src/main.c src/parser.c src/stats.c src/resolve.c src/socket.c src/loop.c
OBJS    = $(SRCS:src/%.c=objs/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lm

objs/%.o: src/%.c include/ft_ping.h
	@mkdir -p objs
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -rf objs

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re