#include "image.h"
#include "segmentation.h"
#include "dataset.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <sys/stat.h>
#include "ccl.h"

/**
 * Concatenate two strings
 */
char *concat(char *a, char *b)
{
    int len = strlen(a) + strlen(b) + 1;
    char *res = malloc(sizeof(char) * len);

    strcpy(res, a);
    strcat(res, b);

    return res;
}

/**
 * Take a set of Img structs containing a path to an image and loads the pixels into the struct.
 * @param images Set of Img structs. Each img has to contain a path.
 * @param dataCount Size of the Img set.
 */
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

/**
 * Create an array of resized images from a list of blocks. The final size is 28*28.
 * @param source: A pointer to the source image.
 * @param chars: A pointer to the linked list of blocks to resize.
 * @param count: The number of blocks.
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

/**
 * Create an array of resized images from a list of images. The final size is 28*28.
 * @param source: A pointer to the source image.
 * @param chars: A pointer to the linked list of images to resize.
 * @param count: The number of blocks.
 * 
 */
Img **images_from_list_of_img(LinkedList *chars, int *count)
{
    int length = list_length(chars);
    Img **images = malloc(sizeof(Img) * length);
    Node *n = chars->start;

    int i = 0;
    for (i = 0; i < length && string[i] != '\0'; i++)
    {
        Block *block = img_make_block(n->data);
        // If the node is a whitespace, skip it
        if (block->label != '\0')
        {
            n = n->next;
            i--;
            continue;
        }
        Img *c = img_resize(n->data, block, 28, 28);
        images[i] = c;
        images[i]->label = string[i];
        n = n->next;
    }
    *count = i;
    return images;
}

/**
 * Open the training images of lines and create training images of chars.
 * @param source: The path to the image to split.
 * @param destination: The path where to save the dataset.
*/
void create_dataset_from_img(char *source, char *destination)
{
    printf("Creating dataset from images...\n");
    // Check source path
    DIR *src = opendir(source);
    if (src == NULL)
    {
        printf("ERROR : source path invalid. Unable to create dataset from it.\n");
        return;
    }

    // Check destination path
    DIR *dest = opendir(destination);
    if (dest == NULL)
        mkdir(destination, 0755);
    else
        closedir(dest);

    struct dirent *file;
    int i = 0;
    char filepath[521];
    // Skip self and parent directories
    readdir(src);
    readdir(src);
    // For each file in dir
    while ((file = readdir(src)) != NULL)
    {
        // Skip hidden files
        if (file->d_name[0] == '.')
            continue;

        // open image of the line
        sprintf(filepath, "%s/%s", source, file->d_name);
        Img *img = img_import(filepath);
        // Split into characters
        // LinkedList *chars = segmentation(img, false);
        LinkedList *chars = ccl_segmentation(img, false);
        // Create images
        int count = 0;
        Img **images = images_from_list_of_img(chars, &count);
        // Img **images = images_from_list(img, chars, &count);

        // Save images
        for (int k = 0; k < count; k++)
        {
            sprintf(filepath, "%s/%u_%d.png", destination, (unsigned char)images[k]->label, i);
            img_save(images[k], filepath);
        }
        i++;
    }
    closedir(src);
    printf("Dataset created at : %s\n", destination);
}

/**
 * Read a dataset. Open every images of the filepath and return a LinkedList of paths.
 * You then need to call dataset_to_pixels to loads the pixels.
 * @param filepath: path to the dataset folder to parse.
 */
LinkedList *read_dataset(char *filepath)
{
    /* Read the dataset without knowing number of files */
    printf("Read dataset : %s\n", filepath);
    LinkedList *images = list_init();
    DIR *dir = opendir(filepath);
    if (dir == NULL)
    {
        printf("ERROR : Dataset filepath does not exist.\n");
        exit(1);
    }

    readdir(dir);
    readdir(dir);
    struct dirent *file;
    int i = 0;
    while ((file = readdir(dir)) != NULL)
    {
        // Skip hidden files
        if (file->d_name[0] == '.')
            continue;
        // Store the file in the img
        Img *image = img_init(28, 28);
        image->filepath = malloc(sizeof(char) * 512);
        sprintf(image->filepath, "%s/%s", filepath, file->d_name);
        int c = atoi(file->d_name); // atoi il fé de la merd
        image->label = c;

        list_insert(images, node_init(image));
        i++;
    }

    return images;
}
