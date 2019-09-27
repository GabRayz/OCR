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

Img *img_init();

Img *img_import(char *filepath);
void img_print(double *pixels, int width, int height);
double *img_grayscale(double *pixels, int size);
void img_save(double *pixels, int width, int height, char *filepath);
#endif
