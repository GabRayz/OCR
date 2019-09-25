#include "image.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

double *img_import(char *filepath)
{
    /* Take the path of the image file. Returns the pixel array in black & white */
    MagickWand *wand = NewMagickWand();
    MagickReadImage(wand, filepath);
    int height = MagickGetImageHeight(wand);
    int width = MagickGetImageWidth(wand);
    // Initialize the pixel array. Each pixel is represented by 3 doubles (RGB)
    double *pixels = malloc(sizeof(double) * width * height * 3);
    for (int i = 0; i < width * height * 3; i++)
    {
        pixels[i] = 0.0;
    }
    // Export pixels from coordinates (0,0) in RGB order. Store in pixels
    MagickExportImagePixels(wand, 0, 0, width, height, "RGB", DoublePixel, pixels);
    // Convert into grayscale
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
    double *grayscale = malloc(sizeof(double) * size / 3);
    int k = 0;
    for (int i = 0; i < size; i = i + 3)
    {
        // Grayscale formula
        grayscale[k] = (0.3 * pixels[i]) + (0.59 * pixels[i + 1]) + (0.11 * pixels[i + 2]);
        k++;
    }

    // Convert into black & white
    float threshold = 0.8;
    for (int i = 0; i < size / 3; i++)
    {
        grayscale[i] = (grayscale[i] < threshold) ? 0 : 1;
    }
    return grayscale;
}

void img_save(double *pixels, int width, int height, char *filepath)
{
    MagickWand *wand = NewMagickWand();
    MagickReadImage(wand, filepath);
    MagickWriteImage(wand, "res.png");
    if (MagickImportImagePixels(wand, 0, 0, width, height, "I", DoublePixel, pixels) == MagickFalse)
    {
        printf("Success\n");
        
        if(MagickWriteImage(wand, filepath) == MagickFalse) {
            printf("Success\n");
        }else {
            printf("Failure :c\n");
        }
    }
    else
    {
        printf("Failure :c\n");
    }
}
