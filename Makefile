CC = gcc

NAME = ocr

SRCS = main.c matrix.c neuralnetwork.c image.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Wextra -std=c99 `pkg-config --cflags MagickWand`

CLIBS = `pkg-config --libs MagickWand`

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(CLIBS)

all: $(NAME)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all
