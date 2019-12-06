#ifndef IMG_H
# define IMG_H

/**
 * Structure representing an image. Can contains a path, a label, the size and the pixels.
 */
typedef struct _Img
{
    char label;
    int width;
    int height;
    double *pixels;
    char *filepath;
} Img;

#include "segmentation.h"

Img *img_init(int width, int height);
void img_delete(Img *image);

Img *img_import(char *filepath);
void print_image(Img *image);
void block_print(Img *source, Block *block);
double *img_grayscale(double *pixels, int size);
void img_bw(double *grayscale, int size, float threshold);
void img_save(Img *img, char *filepath);
Img *img_resize(Img *source, Block *block, int width, int height);

float img_otsu_mu(double *hist, int i, int k);
float img_otsu_w(double *hist, int i, int k);
int img_otsu(double *grayscale, int size);

#endif
