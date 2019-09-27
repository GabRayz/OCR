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
void block_split_vertical(Img *image, Block *block, Block *res1, Block *res2);
Block *remove_top_margin(Img *img, Block *block);
Block *remove_bottom_margin(Img *img, Block *block);
Block *remove_left_margin(Img *img, Block *block);
Block *remove_right_margin(Img *img, Block *block);
//Img resize(Img *image);

#endif
