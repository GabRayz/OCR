#include "image.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ImageMagick-7/MagickWand/MagickWand.h>

char *concat(char *a, char *b)
{
    int len = strlen(a) + strlen(b) + 1;
    char *res = malloc(sizeof(char) * len);

    strcpy(res, a);
    strcat(res, b);

    return res;
}

Img **read_dataset(int dataCount)
{
    /* 
    Read the dataset/training directory to get images names
     */
    Img **images = malloc(sizeof(Img *) * dataCount);
    char *prefix = "dataset/easy/";
    char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir("./dataset/easy/");
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
        images[i] = img_init(28, 28);
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

Img **read_dataset2()
{
    /* 
    Read the dataset/training directory to get images names
     */
    int dataCount = 1016 * 62;
    Img **images = malloc(sizeof(Img *) * dataCount);
    char *prefix = "./dataset/fonts/";
    char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir(prefix);

    assert(dataset != NULL);

    // Skip self and parent directory
    readdir(dataset);
    readdir(dataset);

    int i = 0;
    while ((dir = readdir(dataset)) != NULL && i < 62)
    {
        char *folderName = concat(prefix, dir->d_name);
        char *tmp = concat(folderName, "/");
        DIR *character = opendir(folderName);
        int num = atoi(dir->d_name);
        char label = num < 11 ? num + '0' - 1 : num < 37 ? num + 'A' - 11 : num + 'a' - 37;
        readdir(character);
        readdir(character);

        struct dirent *dir2;
        int j = 0;
        while ((dir2 = readdir(character)) != NULL && j < 1016)
        {
            char *fileName = concat(tmp, dir2->d_name);

            // Store the file in the img
            int index = i * 1016 + j;
            images[index] = img_init(28, 28);
            images[index]->filepath = fileName;
            images[index]->label = label;

            j++;
        }

        free(tmp);
        free(folderName);
        closedir(character);
        i++;
    }

    closedir(dataset);
    return images;
}

void dataset_to_pixels(Img **images, int dataCount)
{
    MagickWand *mw = NewMagickWand();

    for (int i = 0; i < dataCount && images[i]; i++)
    {
        Img *image = images[i];

        if (MagickReadImage(mw, image->filepath) == MagickTrue)
        {
            // printf("File opened successfuly\n");
            MagickAdaptiveResizeImage(mw, 28, 28);
            MagickExportImagePixels(mw, 0, 0, 28, 28, "R", DoublePixel, image->pixels);

            //print_image(image);
        }
        else
            printf("FAILED: %s\n", image->filepath);
    }

    DestroyMagickWand(mw);
}
