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

char **read_dataset(int dataCount)
{
    /* 
    Read the dataset/training directory to get images names
     */

    char *prefix = "dataset/training/";
    char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir("./dataset/training/");
    if (!dataset)
    {
        files[0] = NULL;
        return files;
    }

    // Skip self and parent directory
    readdir(dataset);
    readdir(dataset);

    // For each file up to dataCount
    int i = 0;
    while ((dir = readdir(dataset)) != NULL && i < dataCount)
    {
        // Store the file name in the array
        files[i] = concat(prefix, dir->d_name);
        i++;
    }

    if (i < dataCount)
        files[i] = NULL;

    closedir(dataset);

    return files;
}

void dataset_to_pixels(char *files[], int dataCount)
{
    MagickWand *mw = NewMagickWand();

    for (int i = 0; i < dataCount && files[i]; i++)
    {
        char *file = files[i];

        printf("%s\n", file);

        MagickReadImage(mw, file);
        if (MagickReadImage(mw, file) == MagickTrue)
            printf("File opened successfuly\n");
        else
            printf("FAILED: %s\n", file);
    }

    DestroyMagickWand(mw);
}

int main(/* int argc, char **argv */)
{
    char **files = read_dataset(5);

    dataset_to_pixels(files, 5);

    return 0;
}
