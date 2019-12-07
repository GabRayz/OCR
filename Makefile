CC = gcc

NAME = ocr

SRCS = main.c matrix.c neuralnetwork.c image.c segmentation.c linkedlist.c window.c dataset.c ccl.c

OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Wextra -std=c99 `pkg-config  --cflags MagickWand ` -O3

CLIBS = -lSDL2_image -lSDL2_ttf `sdl2-config --cflags` `sdl2-config --libs` `pkg-config --libs MagickWand ` 

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
