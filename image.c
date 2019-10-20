#include "image.h"
#include "segmentation.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

Img *img_init(int width, int height)
{
    Img *img = malloc(sizeof(Img));

    img->filepath = malloc(sizeof(char *));
    img->pixels = malloc(sizeof(double) * width * height);
    img->width = width;
    img->height = height;

    for (int x = 0; x < width * height; x++)
    {
        img->pixels[x] = 1.0;
    }
    return img;
}

void img_delete(Img *image)
{
    free(image->pixels);
    free(image->filepath);
    free(image);
}

Img *img_import(char *filepath)
{
    /* Take the path of the image file. Returns the pixel array in black & white */
    MagickWand *wand = NewMagickWand();
    if (MagickReadImage(wand, filepath) == MagickFalse)
    {
        printf("Error: Couldn't open image\n");
        exit(1);
    }
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

void print_image(Img *image)
{
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            printf("%d ", (int)image->pixels[y * image->width + x]);
        }
        printf("\n");
    }
}

void block_print(Img *source, Block *block)
{
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            printf("%d ", (int)source->pixels[y * block->width + x]);
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
    float threshold = 0.3;
    for (int i = 0; i < size / 3; i++)
    {
        grayscale[i] = (grayscale[i] < threshold) ? 0 : 1;
    }
    return grayscale;
}

void img_save(Img *img, char *filepath)
{
    MagickWand *wand = NewMagickWand();
    PixelWand *pixel = NewPixelWand();
    MagickNewImage(wand, img->width, img->height, pixel);
    ClearPixelWand(pixel);
    if (MagickImportImagePixels(wand, 0, 0, img->width, img->height, "I", DoublePixel, img->pixels) == MagickTrue)
    {
        if (MagickWriteImage(wand, filepath) == MagickFalse)
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

Img *img_resize(Img *source, Block *block, int width, int height)
{
    int offsetX = (block->height * width / height - block->width) / 2;
    int offsetY = (block->width * height / width - block->height) / 2;

    if (offsetX > 0)
        offsetY = 0;
    else
        offsetX = 0;
    Img *res = img_init(block->width + 2 * offsetX, block->height + 2 * offsetY);

    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            res->pixels[(y + offsetY) * res->width + x + offsetX] = source->pixels[(y + block->y) * source->width + x + block->x];
        }
    }

    MagickWand *wand = NewMagickWand();

    PixelWand *pixel = NewPixelWand();
    MagickNewImage(wand, res->width, res->height, pixel);
    ClearPixelWand(pixel);

    MagickImportImagePixels(wand, 0, 0, res->width, res->height, "I", DoublePixel, res->pixels);

    MagickAdaptiveResizeImage(wand, width, height);
    Img *resized = img_init(width, height);
    MagickExportImagePixels(wand, 0, 0, width, height, "I", DoublePixel, resized->pixels);
    return resized;

    //return res;
}
