#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "matrix.h"
#include "image.h"
#include "neuralnetwork.h"
#include "separation.h"
#include "window.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <gtk/gtk.h>

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
            // printf("File opened successfuly\n");
            MagickExportImagePixels(mw, 0, 0, 28, 28, "R", DoublePixel, image->pixels);

            //print_image(image);
        }
        else
            printf("FAILED: %s\n", image->filepath);
    }

    DestroyMagickWand(mw);
}

#define COUNT 36

void train(NeuralNetwork *nn, Img **images, int cycles, int learn)
{
    /* 
    Train the neural network with the given set of images
    */
    double *results = malloc(sizeof(double) * cycles);

    for (int i = 0; i < cycles; i++)
    {
        Img *img = images[i % COUNT];

        nn_compute(nn, img->pixels, (int)img->label);

        if (learn)
            nn_backProp(nn);
        //    printf("Result : %c, Label : %c, cost : %lf\n", nn_getResult(nn), img->label, nn_getCost(nn));
        results[i] = (nn_getResult(nn) == img->label) ? 1.0 : 0.0;
    }

    double sum = 0;
    for (int i = 0; i < cycles; i++)
    {
        sum += results[i];
    }

    printf("Accuracy : %lf\n", (sum / cycles) * 100);
}

Block *list_get_index(LinkedList *list, int index)
{
    Node *node = list->start;
    int i = 0;
    for (i = 0; i < index && node; i++)
    {
        node = node->next;
    }
    if (i == index)
        return node->block;
    else
        return NULL;
}

LinkedList *split_paragraphs(Img *source)
{
   /*
    Takes a source image and creates blocks, each one containing a paragraph.
    returns the linked list of blocks
   */

}

int main(int argc, char **argv)
{
    // open_window(argc,argv);
    // return 0;
    MagickWandGenesis();
    Img *img = img_import("dataset/images/paragraphes.jpeg");
    Block *block = img_make_block(img);

    Block *left = block_init();
    Block *right = block_init();
    block_split_vertical(img, block, left, right);

    // Block *top = block_init();
    // Block *bottom = block_init();
    // block_split_horizontal(img, left, top, bottom);

    remove_white_margin(img, left);
    LinkedList *list = line_split(img, left);

    LinkedList *chars = character_split(img, list->start->block);

    remove_white_margin(img, list_get_index(chars, 6));

    Img *c = img_from_block(img, list_get_index(chars, 6));
    Img *res = img_resize(img, list_get_index(chars, 6), 28, 28);

    img_save(res->pixels, res->width, res->height, "res.png");
    // img_save(c->pixels, c->width, c->height, "res.png");
    // return 0;
    int cycles = 10000;
    Img **images = read_dataset(COUNT);

    dataset_to_pixels(images, COUNT);

    int layerSizes[] = {784, 20, 93};

    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);

    train(nn, images, cycles, 1);
    // train(nn, images, cycles, 0);

    nn_compute(nn, res->pixels, 'l');
    printf("%c\n", nn_getResult(nn));
    return 0;
}
