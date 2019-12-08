#include "image.h"
#include "segmentation.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include "matrix.h"

Img *img_init(int width, int height)
{
    Img *img = malloc(sizeof(Img));

    img->filepath = malloc(sizeof(char *));
    img->pixels = malloc(sizeof(double) * width * height);
    img->width = width;
    img->height = height;
    img->label = '\0';

    for (int x = 0; x < width * height; x++)
    {
        img->pixels[x] = 1.0;
    }
    return img;
}

void img_delete(Img *image)
{
    free(image->pixels);
    // free(image->filepath);
    free(image);
}

/**
 * Apply a filter to the image.
 * @param source: Source image to apply the filter to
 * @param filter: Matrix representation of the filter.
 * Modifies the image in place
 */
void img_apply_filter(Img *source, double *filter, int divisor)
{
    double *new = calloc(source->width * source->height, sizeof(double));
    for (int y = 0; y < source->height; y++)
    {
        for (int x = 0; x < source->width; x++)
        {
            new[y * source->width + x] = source->pixels[y * source->width + x];
        }
    }
    

    for (int y = 1; y < source->height - 1; y++)
    {
        for (int x = 1; x < source->width - 1; x++)
        {
            // For each pixel, go through neighbors
            int k = 0;
            double res = 0;
            for (int j = y - 1; j < y + 2; j++)
            {
                for (int i = x - 1; i < x + 2; i++)
                {
                    // Compute the filter result
                    // printf("%lf\n", source->pixels[(y + j) * source->width + (x + i)]);
                    res += source->pixels[j * source->width + i] * filter[k];
                    k++;
                }
            }
            // Apply
            // printf("%lf\n", res / 16);
            new[y * source->width + x] = res / (double)divisor;
        }
    }
    free(source->pixels);
    source->pixels = new;
}

Img *img_import(char *filepath)
{
    /* Take the path of the image file. Returns the pixel array in black & white */
    MagickWand *wand = NewMagickWand();
    if (MagickReadImage(wand, filepath) == MagickFalse)
    {
        printf("Error: Couldn't open image\n");
        ExceptionType type = MagickGetExceptionType(wand);
        printf("%s\n", MagickGetException(wand, &type));
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
    // Apply filter
    free(pixels);
    printf("before copy: %s\n", filepath);
    Img *res = img_init(width, height);
    res->pixels = image;
    res->filepath = filepath;
    // Create filter matrix
    // double filter[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
    // img_apply_filter(res, filter, 16);

    // Turn the image in B&W
    img_bw(res->pixels, width * height);
    img_save(res, "res/otsu.png");

    // Remove margins
    Block *block = img_make_block(res);
    remove_white_margin(res, block);
    Img *trim = img_from_block(res, block);
    trim->filepath = filepath;
    //img_delete(res);
    return trim;
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
        grayscale[i] *= 255;
    }

    return grayscale;
}

int img_otsu(double *grayscale, int size)
{
    // Create the histogram
    double *hist = calloc(256, sizeof(double));
    for (int i = 1; i < size; i++)
    {
        hist[(int)grayscale[i]] += 1 / (float)size;
    }

    // Search for max
    float max = 0;
    int threshold = 0;
    for (int k = 1; k < 255; k++)
    {
        float w0 = img_otsu_w(hist, 0, k);
        float w1 = img_otsu_w(hist, k, 255);
        float tmp = img_otsu_mu(hist, 0, k) - img_otsu_mu(hist, k, 255);
        float s = w0 * w1 * tmp * tmp;
        if (s > max)
        {
            max = s;
            threshold = k;
        }
    }
    free(hist);

    return threshold;
}

float img_otsu_mu(double *hist, int i, int k)
{
    float res = 0;
    for (int j = i; j < k; j++)
    {
        res += j * hist[j];
    }
    return res / img_otsu_w(hist, i, k);
}

float img_otsu_w(double *hist, int i, int k)
{
    float res = 0;
    for (; i < k; i++)
    {
        res += hist[i];
    }
    return res;
}

void img_bw(double *grayscale, int size)
{
    int otsu = img_otsu(grayscale, size);
    printf("otsu: %d\n", otsu);
    int threshold = (otsu > 250) ? 76: otsu;
    for (int i = 0; i < size; i++)
    {
        grayscale[i] = (grayscale[i] < threshold) ? 0 : 1;
    }
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
}
