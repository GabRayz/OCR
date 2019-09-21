#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "matrix.h"
#include "neuralnetwork.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

char *concat(char *a, char *b)
{
    int len = strlen(a) + strlen(b) + 1;
    char *res = malloc(sizeof(char) * len);

    strcpy(res, a);
    strcat(res, b);

    return res;
}

typedef struct Img
{
    char label;
    double *pixels;
    char *filepath;
} Img;

Img *img_init()
{
    Img *img = malloc(sizeof(Img));

    img->filepath = malloc(sizeof(char *));
    img->pixels = malloc(sizeof(double) * 784);
    return img;
}

Img **read_dataset(int dataCount)
{
    /* 
    Read the dataset/training directory to get images names
     */
    Img **images = malloc(sizeof(Img *) * dataCount);
    char *prefix = "dataset/training/";
    char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir("./dataset/training/");
    if (!dataset)
    {
        images[0] = NULL;
        return images;
    }

    // Skip self and parent directory
    readdir(dataset);
    readdir(dataset);

    // For each file up to dataCount
    int i = 0;
    while ((dir = readdir(dataset)) != NULL && i < dataCount)
    {
        // Store the file in the img
        images[i] = img_init();
        images[i]->filepath = concat(prefix, dir->d_name);
        // strcpy(images[i]->label, dir->d_name[0]);
        images[i]->label = dir->d_name[0];
        i++;
    }

    if (i < dataCount)
        files[i] = NULL;

    closedir(dataset);
    return images;
}

void print_image(Img *image)
{
    for (int y = 0; y < 28; y++)
    {
        for (int x = 0; x < 28; x++)
        {
            printf("%d ", (int)image->pixels[y * 28 + x]);
        }
        printf("\n");
    }
}

void dataset_to_pixels(Img **images, int dataCount)
{
    MagickWand *mw = NewMagickWand();

    for (int i = 0; i < dataCount && images[i]; i++)
    {
        Img *image = images[i];
        
        if (MagickReadImage(mw, image->filepath) == MagickTrue)
        {
            printf("File opened successfuly\n");
            MagickExportImagePixels(mw, 0, 0, 28, 28, "R", DoublePixel, image->pixels);
            
            // print_image(image)
        }
        else
            printf("FAILED: %s\n", image->filepath);
    }

    DestroyMagickWand(mw);
}

int main(/* int argc, char **argv */)
{
    printf("%d\n", (int)'.'); // 46
    printf("%c\n", 46 + '0'); // ^
    for (int i = 0; i < 94; i++)
    {
        printf("%c", i - 15 + '0');
    }
    return 0;

    Img **images = read_dataset(5);

    dataset_to_pixels(images, 5);

    return 0;
}
