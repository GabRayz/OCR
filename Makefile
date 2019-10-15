CC = gcc

NAME = ocr

SRCS = main.c matrix.c neuralnetwork.c image.c separation.c linkedlist.c window.c dataset.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Wextra -std=c99 `pkg-config --cflags MagickWand` -O3

CLIBS = `pkg-config --libs MagickWand sdl` -lSDL_image -lSDL_ttf

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(CLIBS) -lm

all: $(NAME)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all
