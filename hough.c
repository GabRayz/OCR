#include "hough.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ImageMagick-7/MagickWand/MagickWand.h>

#define PRECISION_HOUGH 0.01
#define SIZE_HOUGH (90 / PRECISION_HOUGH)

Img *hough(Img *source)
{
    int *hist = calloc(SIZE_HOUGH, sizeof(int));
    // For each white pixel
    for (int y = 0; y < source->height; y += 50)
    {
        for (int x = 0; x < source->width; x += 50)
        {
            if (source->pixels[y * source->width + x] == 1)
                hough_process_point(source, hist, x, y);
        }
    }
    double angle = (get_max(hist) - M_PI / 2) / M_PI * 180;
    printf("Image rotation angle : %lf\n", angle);
    free(hist);
    return apply_angle(source, angle);
}

Img *apply_angle(Img *source, double angle)
{
    MagickWand *wand = NewMagickWand();
    PixelWand *pixel = NewPixelWand();
    PixelSetColor(pixel, "white");
    MagickNewImage(wand, source->width, source->height, pixel);

    MagickImportImagePixels(wand, 0, 0, source->width, source->height, "I", DoublePixel, source->pixels);
    MagickRotateImage(wand, pixel, -angle);
    ClearPixelWand(pixel);
    // Save the image
    MagickWriteImage(wand, "res/res.png");

    int width = MagickGetImageWidth(wand);
    int height = MagickGetImageHeight(wand);
    Img *rotated = img_init(width, height);
    MagickExportImagePixels(wand, 0, 0, width, height, "I", DoublePixel, rotated->pixels);
    return rotated;
}

double get_max(int *hist)
{
    double max = 0;
    int maxIndex = 0;
    for (int i = 0; i < SIZE_HOUGH; i++)
    {
        if (hist[i] > max)
        {
            max = hist[i];
            maxIndex = i;
            // printf("dede %d\n", i);
        }
    }
    return index_to_angle(maxIndex);
}

void hough_process_point(Img *source, int *hist, int x, int y)
{
    for (double t = 45; t < 135; t += PRECISION_HOUGH)
    {
        double theta = t * (double)(M_PI / 180.0);

        double r = x * cos(theta) + y * sin(theta);
        draw_line(source, hist, r, theta);
    }
    // img_save(source, "res/hough.png");
}

void draw_line(Img *source, int *hist, double r, double theta)
{
    for (int x = 0; x < source->width; x++)
    {
        int y = (r - x * cos(theta)) / sin(theta);
        if (y >= 0 && y < source->height)
        {
            // If there is a black pixel, stop here
            if (source->pixels[y * source->width + x] == 0)
                return;
        }
    }

    // for (int x = 0; x < source->width; x++)
    // {
    //     int y = (r - x * cos(theta)) / sin(theta);
    //     if (y >= 0 && y < source->height)
    //     {
    //         // If there is a black pixel, stop here
    //         source->pixels[y * source->width + x] = 0.6;
    //     }
    // }
    // If the line is white, register its parameters
    hist[angle_to_index(theta)] += 1;
}

int grosso_merdo(int a, int b)
{
    return -5 < a - b && a - b < 5;
}

/**
 * Convert a angle in radian to an index.
 */
int angle_to_index(double theta)
{
    int res = (theta - M_PI / 4) / (M_PI / 2) * SIZE_HOUGH;
    assert(res >= 0 && res < SIZE_HOUGH);
    return res;
}

double index_to_angle(int index)
{
    double res = (index / SIZE_HOUGH) * (M_PI / 2) + M_PI / 4;
    assert(res >= M_PI / 4 && res <= M_PI * 3 / 4);
    return res;
}
