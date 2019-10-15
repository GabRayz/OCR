#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "matrix.h"
#include "image.h"
#include "neuralnetwork.h"
#include "separation.h"
#include "window.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <assert.h>
#include "dataset.h"


// #define COUNT 36

void debug() {
    printf("Debug\n");
}

void train(NeuralNetwork *nn, Img **images, int images_count, int cycles, int learn)
{
    /* 
    Train the neural network with the given set of images
    */
    printf("Training...\n");
    double *results = malloc(sizeof(double) * cycles);
    
    for (int i = 0; i < cycles; i++)
    {
        unsigned int index = learn ? rand() % images_count : i % images_count;
        Img *img = images[index];
        // print_image(img);
        // printf("%c\n", img->label);
        // printf("%d\n", index);

        nn_compute(nn, img->pixels, (int)img->label);
        if (learn)
            nn_backProp(nn);
        
        results[i] = (nn_getResult(nn) == img->label) ? 1.0 : 0.0;
        // if (!learn)
        //     printf("%c", nn_getResult(nn));
    }

    double sum = 0;
    for (int i = 0; i < cycles; i++)
    {
        sum += results[i];
    }

    printf("Accuracy : %lf\n", (sum / cycles) * 100);
}

NeuralNetwork *create_nn()
{
    // Create a neural network, initialize it randomly, and make it learn
    int cycles = 100000;
    Img **images = read_dataset2();
    printf("Loaded paths, loading images...\n");
    dataset_to_pixels(images, 1016 * 36);

    int* layerSizes = malloc(sizeof(int) * 3);
    layerSizes[0] = 784;
    layerSizes[1] = 256;
    layerSizes[2] = 93;

    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);
    printf("Let's train !\n");

    train(nn, images, 1016 * 36, cycles, 1);
    train(nn, images, 1016 * 36, cycles, 0);
    return nn;
}

NeuralNetwork *create_nn_from_img(Img **images, int images_count)
{
    // Create a neural network, initialize it randomly, and make it learn
    printf("Creating the neural network\n");
    int cycles = 10000;
    int* layerSizes = malloc(sizeof(int) * 3);
    layerSizes[0] = 784;
    layerSizes[1] = 25;
    layerSizes[2] = 93;
    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);
    train(nn, images, images_count, cycles, 1);
    train(nn, images, images_count, cycles, 0);
    return nn;
}

LinkedList *segmentation(Img *source)
{
    Block *block = img_make_block(source);
    LinkedList *paragraphs = block_split_vertical(source, block);
    LinkedList *lines = list_init();
    Node *p = paragraphs->start;
    while (p)
    {
        lines = list_concat(lines, line_split(source, p->block));
        p = p->next;
    }

    LinkedList *chars = list_init();
    Node *l = lines->start;
    while (l)
    {
        chars = list_concat(chars, character_split(source, l->block));
        l = l->next;
    }

    return chars;
}

char *send_to_cerveau(Img *source, LinkedList *chars, NeuralNetwork *nn)
{
    char *res = malloc(sizeof(char) * list_length(chars));
    Node *n = chars->start;
    int i = 0;
    while (n)
    {
        remove_white_margin(source, n->block);
        Img *resized = img_resize(source, n->block, 28, 28);
        // Send to the neural network
        nn_compute(nn, resized->pixels, 100);
        res[i] = nn_getResult(nn);
        n = n->next;
        i++;
    }

    return res;
}

Img **images_from_list(Img *source, LinkedList *chars, char *label, int *count)
{
    int length = list_length(chars);
    Img **images = malloc(sizeof(Img) * length);
    Node *n = chars->start;
    // printf("%d\n", strlen(label));
    int i = 0;
    for (i = 0; i < length && label[i] != '\0'; i++)
    {
        Img *c = img_resize(source, n->block, 28, 28);
        // char truc[] = {i / 100 + 48, (i / 10) % 10 + 48, i % 10 + 48, '\0'};
        // img_save(c, concat(concat("res/", truc), ".png"));
        images[i] = c;
        images[i]->label = label[i];
        n = n->next;
    }
    // printf("%d\n", i);
    *count = i;
    return images;
}

int main()
{
    init_window();
    return 0;
    MagickWandGenesis();
    printf("Importing image...\n");
    Img *source = img_import("dataset/images/spaced.png");
    printf("Segmenting image...\n");
    LinkedList *chars = segmentation(source);
    
    printf("Creating NN...\n");
    NeuralNetwork *nn = create_nn();

    return 0;
}
