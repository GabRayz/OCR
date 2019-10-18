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

NeuralNetwork *create_nn(char *filepath)
{
    // Create a neural network, initialize it randomly, and make it learn
    int cycles = 50000;
    LinkedList *list = read_dataset(filepath);

    int dataCount = list_length(list);
    Img **images = (Img **)list_to_array(list);
    dataset_to_pixels(images, dataCount);
    printf("Loaded paths, loading images...\n");

    int *layerSizes = malloc(sizeof(int) * 4);
    layerSizes[0] = 784;
    layerSizes[1] = 384;
    layerSizes[2] = 256;
    layerSizes[3] = 93;

    NeuralNetwork *nn = nn_init(layerSizes, 4);
    nn_setupRandom(nn);
    printf("Let's train !\n");

    train(nn, images, dataCount, cycles);
    // train(nn, images, dataCount, 10000, 0);
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
    NeuralNetwork *nn = create_nn("dataset/training/set1");
    nn_saveBinary(nn, "save/cervo1");
}

int read_image(int argc, char **argv)
{
    Img *source = img_import("dataset/images/spaced.png");
    LinkedList *chars = segmentation(source);
    NeuralNetwork *nn = nn_load("save/cervo1");
    char *res = send_to_cerveau(source, chars, nn);
    printf("%s\n", res);
}

int main(int argc, char **argv)
{
    init_window();
    return 0;
}
