CC = gcc

NAME = ocr

SRCS = main.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Wextra -Werror -O

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

all: $(NAME)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all
