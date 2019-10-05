#include "image.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

Img *img_init(int width, int height)
{
    Img *img = malloc(sizeof(Img));

    img->filepath = malloc(sizeof(char *));
    img->pixels = malloc(sizeof(double) * width * height);
    img->width = width;
    img->height = height;
    return img;
}

void img_delete(Img *image) {
    free(image->pixels);
    free(image->filepath);
    free(image);
}

Img *img_import(char *filepath)
{
    /* Take the path of the image file. Returns the pixel array in black & white */
    MagickWand *wand = NewMagickWand();
    MagickReadImage(wand, filepath);
    int height = MagickGetImageHeight(wand);
    int width = MagickGetImageWidth(wand);
    // Initialize the pixel array. Each pixel is represented by 3 doubles (RGB)
    double *pixels = malloc(sizeof(double) * width * height * 3);

    // Export pixels from coordinates (0,0) in RGB order. Store in pixels
    MagickExportImagePixels(wand, 0, 0, width, height, "RGB", DoublePixel, pixels);
    // Convert into grayscale
    double *image = img_grayscale(pixels, width * height * 3);

    //
    Img *res = img_init(width, height);
    res->pixels = image;
    res->filepath = filepath;

    return res;
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
    for (int i = 0; i < size / 3; i++)
    {
        // Grayscale formula
        grayscale[i] = (0.3 * pixels[i * 3]) + (0.59 * pixels[i * 3 + 1]) + (0.11 * pixels[i * 3 + 2]);
    }

    // Convert into black & white
    float threshold = 0.5;
    for (int i = 0; i < size / 3; i++)
    {
        grayscale[i] = (grayscale[i] < threshold) ? 0 : 1;
    }
    return grayscale;
}

void img_save(double *pixels, int width, int height, char *filepath)
{
    MagickWand *wand = NewMagickWand();
    PixelWand *pixel = NewPixelWand();
    MagickNewImage(wand, width, height, pixel);
    ClearPixelWand(pixel);
    if (MagickImportImagePixels(wand, 0, 0, width, height, "I", DoublePixel, pixels) == MagickTrue)
    {
        if (MagickWriteImage(wand, filepath) == MagickTrue)
        {
            printf("Image saved successfuly\n");
        }
        else
        {
            printf("Failure :c\n");
            ExceptionType type = MagickGetExceptionType(wand);
            printf("%s\n", MagickGetException(wand, &type));
        }
    }
    else
    {
        printf("Failure :c\n");
        ExceptionType type = MagickGetExceptionType(wand);
        printf("%s\n", MagickGetException(wand, &type));
    }
    ClearMagickWand(wand);
}
