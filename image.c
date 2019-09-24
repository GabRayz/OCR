#include "image.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

double *img_import(char *filepath)
{
    MagickWand *wand = NewMagickWand();
    MagickReadImage(wand, filepath);
    int height = MagickGetImageHeight(wand);
    int width = MagickGetImageWidth(wand);
    double *pixels = malloc(sizeof(double) * width * height * 3);
    for (int i = 0; i < width * height * 3; i++)
    {
        pixels[i] = 0.0;
    }
    
    MagickExportImagePixels(wand, 0, 0, width, height, "RGB", DoublePixel, pixels);
    double *image = img_grayscale(pixels, width * height * 3);
    return image;
}

void img_print(double *pixels, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            printf("%lf ", pixels[y * width + x]);
        }
        printf("\n");
    }
}

double *img_grayscale(double *pixels, int size)
{
    // Convert into grayscale
    double *grayscale = malloc(sizeof(double) *size/3);
    int k = 0;
    for (int i = 0; i < size; i = i + 3)
    {
        grayscale[k] = (0.3*pixels[i])+(0.59*pixels[i+1])+(0.11*pixels[i+2]);
        k++;
    }

    // Convert into black & white
    for (int i = 0; i < size / 3; i++)
    {
        grayscale[i] = (grayscale[i] < 0.8) ? 0 : 1;
    }
    return grayscale;
}
