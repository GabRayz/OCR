#ifndef SEPARATION_H

# define SEPARATION_H
#include "image.h"

typedef struct _Block {
    int x;
    int y;
    int width;
    int height;
} Block;

Block *block_init();
void block_delete(Block *block);
void remove_white_margin(Img *image, Block *block);
double vertical_white_rate(Img *image, Block *block, int x);
double horizontal_white_rate(Img *image, Block *block, int y);
Block *img_make_block(Img *image);
Img *img_from_block(Img *source, Block *block);
void remove_white_margin(Img *image, Block *block);
Img resize(Img *image);

#endif
