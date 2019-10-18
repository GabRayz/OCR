#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "main.h"
#include "matrix.h"
#include "image.h"
#include "neuralnetwork.h"
#include "segmentation.h"
#include "window.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>
#include <assert.h>
#include "dataset.h"

void train(NeuralNetwork *nn, Img **images, int images_count, int cycles, int learn)
{
    /* 
    Train the neural network with the given set of images
    */
    printf("Training...\n");
    fputs("\e[?25l", stdout); /* hide the cursor */
    double *results = malloc(sizeof(double) * cycles);
    for (int i = 0; i < cycles; i++)
    {

        printf("\r%d / %d", i + 1, cycles);
        unsigned int index = learn ? rand() % images_count : i % images_count;
        Img *img = images[index];
        // print_image(img);
        // printf("%c %d\n", img->label, img->label);

        nn_compute(nn, img->pixels, (int)img->label);
        if (learn)
            nn_backProp(nn);

        results[i] = (nn_getResult(nn) == img->label) ? 1.0 : 0.0;
        // if (!learn)
        //     printf("%c", nn_getResult(nn));
    }
    fputs("\e[?25h", stdout); /* show the cursor */
    printf("\n");

    double sum = 0;
    for (int i = 0; i < cycles; i++)
    {
        sum += results[i];
    }

    printf("Accuracy : %lf\n", (sum / cycles) * 100);
}

NeuralNetwork *create_nn(char *filepath)
{
    // Create a neural network, initialize it randomly, and make it learn
    int cycles = 15000;
    LinkedList *list = read_dataset3(filepath);

    int dataCount = list_length(list);
    Img **images = (Img **)list_to_array(list);
    dataset_to_pixels(images, dataCount);
    printf("Loaded paths, loading images...\n");

    int *layerSizes = malloc(sizeof(int) * 3);
    layerSizes[0] = 784;
    layerSizes[1] = 256;
    layerSizes[2] = 93;

    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);
    printf("Let's train !\n");

    train(nn, images, dataCount, cycles, 1);
    train(nn, images, dataCount, 10000, 0);
    return nn;
}

NeuralNetwork *create_nn_from_img(Img **images, int images_count)
{
    // Create a neural network, initialize it randomly, and make it learn
    printf("Creating the neural network\n");
    int cycles = 10000;
    int *layerSizes = malloc(sizeof(int) * 3);
    layerSizes[0] = 784;
    layerSizes[1] = 25;
    layerSizes[2] = 93;
    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);
    train(nn, images, images_count, cycles, 1);
    train(nn, images, images_count, 1000, 0);
    return nn;
}

char *send_to_cerveau(Img *source, LinkedList *chars, NeuralNetwork *nn)
{
    char *res = malloc(sizeof(char) * list_length(chars) + 1);
    Node *n = chars->start;
    int i = 0;
    while (n)
    {
        remove_white_margin(source, n->data);
        Img *resized = img_resize(source, n->data, 28, 28);
        // Send to the neural network
        nn_compute(nn, resized->pixels, 100);
        res[i] = nn_getResult(nn);
        n = n->next;
        i++;
    }
    res[i] = '\0';

    return res;
}

int write_dataset(int argc, char **argv)
{
    create_dataset_from_img("dataset/images/training", "dataset/training/set1");
}

int learn(int argc, char **argv)
{
    create_nn("dataset/training/set1");
}

int main(int argc, char **argv)
{
    argv;
    MagickWandGenesis();
    write_dataset(argc, argv);
    learn(argc, argv);
    return 0;
}
