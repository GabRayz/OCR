CC = gcc

NAME = ocr

SRCS = main.c matrix.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Wextra -std=c99

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
