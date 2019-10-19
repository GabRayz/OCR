#ifndef IMG_H
# define IMG_H

typedef struct _Img
{
    char label;
    int width;
    int height;
    double *pixels;
    char *filepath;
} Img;

#include "segmentation.h"

Img *img_init();

Img *img_import(char *filepath);
void print_image(Img *image);
void block_print(Img *source, Block *block);
double *img_grayscale(double *pixels, int size);
void img_save(Img *img, char *filepath);
Img *img_resize(Img *source, Block *block, int width, int height);
#endif
