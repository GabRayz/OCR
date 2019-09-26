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
Img resize(Img *image);
