#include "image.h"
#include "segmentation.h"
#include "dataset.h"
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
    dataset_to_pixels(images, dataCount);
    return images;
}

Img **read_dataset2()
{
    /* 
    Read the dataset/training directory to get images names
     */
    printf("Reading dataset...\n");

    int dataCount = 1016 * 62;
    Img **images = malloc(sizeof(Img *) * dataCount);
    char *prefix = "./dataset/fonts/";
    // char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir(prefix);

    assert(dataset != NULL);

    // Skip self and parent directory
    readdir(dataset);
    readdir(dataset);

    int i = 0;
    while ((dir = readdir(dataset)) != NULL && i < 62)
    {
        // Prevent hiddent files
        if (dir->d_name[0] == '.')
            continue;

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
            // Prevent hiddent files
            if (dir->d_name[0] == '.')
                continue;
            char *fileName = concat(tmp, dir2->d_name);

            // Store the file in the img
            int index = i * 1016 + j;
            images[index] = img_init(128, 128);
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
    dataset_to_pixels(images, dataCount);
    return images;
}

void dataset_to_pixels(Img **images, int dataCount)
{

    fputs("\e[?25l", stdout); /* hide the cursor */

    for (int i = 0; i < dataCount && images[i]; i++)
    {

        printf("\r%d / %d", i + 1, dataCount);
        MagickWand *mw = NewMagickWand();
        Img *image = images[i];

        if (MagickReadImage(mw, image->filepath) == MagickTrue)
        {
            // printf("File opened successfuly\n");
            // MagickAdaptiveResizeImage(mw, 28, 28);
            MagickExportImagePixels(mw, 0, 0, MagickGetImageWidth(mw), MagickGetImageHeight(mw), "R", DoublePixel, image->pixels);

            //print_image(image);
        }
        else
            printf("FAILED: %s\n", image->filepath);
        DestroyMagickWand(mw);
    }

    fputs("\e[?25h", stdout); /* show the cursor */
    printf("\n");
}
const char *string = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'()-_.,?!:;";

/* 
 * Create an array of resized images from a list of blocks
 * 
*/
Img **images_from_list(Img *source, LinkedList *chars, int *count)
{
    int length = list_length(chars);
    Img **images = malloc(sizeof(Img) * length);
    Node *n = chars->start;

    int i = 0;
    for (i = 0; i < length && string[i] != '\0'; i++)
    {
        Img *c = img_resize(source, n->data, 28, 28);
        images[i] = c;
        images[i]->label = string[i];
        n = n->next;
    }
    *count = i;
    return images;
}

/* 
 * Open the training images of lines and create training images of chars
*/
void create_dataset_from_img(char* source, char *destination)
{
    printf("Creating dataset from images...\n");
    // Open files
    DIR *dir = opendir(source);
    struct dirent *file;
    int i = 0;
    char filepath[256];
    readdir(dir);
    readdir(dir);
    while ((file = readdir(dir)) != NULL)
    {
        printf("open image\n");
        // open image of the line
        sprintf(filepath, "%s/%s", source, file->d_name);
        Img *img = img_import(filepath);
        // Split into characters
        LinkedList *chars = segmentation(img);
        // Create images
        int count = 0;
        Img **images = images_from_list(img, chars, &count);
        

        // Save images
        for (int k = 0; k < count; k++)
        {
            sprintf(filepath, "%s/%u_%d.png", destination, (unsigned char)images[k]->label, i);
            img_save(images[k], filepath);
        }
        i++;
    }
}

LinkedList *read_dataset3(char *filepath)
{
    /* Read the dataset without knowing number of files */

    LinkedList *images = list_init();
    DIR *dir = opendir(filepath);

    readdir(dir);
    readdir(dir);
    struct dirent *file;
    int i = 0;
    while ((file = readdir(dir)) != NULL)
    {
        // Store the file in the img
        Img *image = img_init(28, 28);
        image->filepath = malloc(sizeof(char) * 256);
        sprintf(image->filepath, "%s/%s", filepath, file->d_name);
        int c = atoi(file->d_name); // atoi il fé de la merd
        image->label = c;

        list_insert(images, node_init(image));
        i++;
    }

    // Img ** list = (Img **)list_to_array(images);
    //dataset_to_pixels(list, i);
    return images;
}
