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

NeuralNetwork *create_nn(char *filepath, int cycles)
{
    // Create a neural network, initialize it randomly, and make it lear
    LinkedList *list = read_dataset(filepath);

    int dataCount = list_length(list);
    Img **images = (Img **)list_to_array(list);
    dataset_to_pixels(images, dataCount);

    int *layerSizes = malloc(sizeof(int) * 3);
    layerSizes[0] = 784;
    layerSizes[1] = 256;
    layerSizes[2] = 93;

    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);

    train(nn, images, dataCount, cycles);
    return nn;
}

char *send_to_cerveau(Img *source, LinkedList *chars, NeuralNetwork *nn)
{
    char *res = malloc(sizeof(char) * list_length(chars) + 1);
    Node *n = chars->start;
    int i = 0;
    while (n)
    {
        Block *block = n->data;

        if (block->label == '\0')
        {
            remove_white_margin(source, block);
            Img *resized = img_resize(source, block, 28, 28);

            // Send to the neural network
            // print_image(resized);
            nn_compute(nn, resized->pixels, 100);
            res[i] = nn_getResult(nn);
            // printf("%c\n", res[i]);
        }
        else
        {
            res[i] = block->label;
        }

        n = n->next;
        i++;
    }
    res[i] = '\0';

    return res;
}

int write_dataset(int argc, char **argv)
{
    // If there is no more arguments, take default values
    if (argc == 2)
    {
        create_dataset_from_img("dataset/images/training", "dataset/training/set1");
        return 0;
    }
    if (argc != 4)
    {
        printf("Usage : ./ocr write_dataset {path to source dir} {path to training data dir}\n");
        return 1;
    }
    else
    {
        create_dataset_from_img(argv[2], argv[3]);
    }
    return 0;
}

int learn(int argc, char **argv)
{
    // If there is no more args, take default values
    if (argc == 2)
    {
        NeuralNetwork *nn = create_nn("dataset/training/set1", 50000);
        nn_saveBinary(nn, "save/cervo1");
        return 0;
    }
    else if (argc == 4 || argc == 5)
    {
        int cycles = atoi(argv[3]);
        NeuralNetwork *nn = create_nn(argv[2], cycles);
        if (argc == 5)
        {
            nn_saveBinary(nn, argv[4]);
        }
        return 0;
    }
    printf("Usage : ./ocr learn {path to dataset directory} {cycles} [saving path]\n");
    return 1;
}

int read_image(int argc, char **argv)
{
    if (argc == 4)
    {
        printf("Importing image...\n");
        Img *source = img_import(argv[3]);
        LinkedList *chars = segmentation(source, true);

        NeuralNetwork *nn = nn_load(argv[2]);
        char *res = send_to_cerveau(source, chars, nn);
        printf("%s\n", res);
        return 0;
    }
    printf("Usage : ./ocr read {path to neural network} {path to image}\n");
    printf("Exemple : ./ocr read save/Helve dataset/images/lorem.png\n");
    return 1;
}

int improve(int arc, char **argv)
{
    NeuralNetwork *nn = nn_load("save/cervo1");

    LinkedList *list = read_dataset("dataset/training/set1");

    int dataCount = list_length(list);
    Img **images = (Img **)list_to_array(list);
    dataset_to_pixels(images, dataCount);

    int cycles = atoi(argv[2]);
    train(nn, images, dataCount, cycles);
    nn_saveBinary(nn, "save/cervo1");
}

int main(int argc, char **argv)
{
    if (argc == 1)
        init_window();
    else
    {
        if (strcmp(argv[1], "write_dataset") == 0)
            write_dataset(argc, argv);
        else if (strcmp(argv[1], "learn") == 0)
            learn(argc, argv);
        else if (strcmp(argv[1], "read") == 0)
            read_image(argc, argv);
        else if (strcmp(argv[1], "improve") == 0)
            improve(argc, argv);
        else
            printf("Usage: ./ocr write_dataset|learn|read|improve\n");
    }
    return 0;
}
