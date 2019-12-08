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
#include <sys/stat.h>
#include "ccl.h"
#include "spellchek.h"
#include "hough.h"

NeuralNetwork *create_nn(char *filepath, int cycles, int count)
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

    train(nn, images, dataCount, cycles, count);
    return nn;
}

/**
 * Send characters to a neural network for recognition.
 * It removes the margin of each characters and resize them.
 * @param source: source image of the text
 * @param chars: linked list of characters (Blocks)
 * @param nn: the neural network to use
 */
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
            nn_compute(nn, resized->pixels);
            res[i] = nn_getResult(nn);
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

char *send_images_to_cerveau(LinkedList *images, NeuralNetwork *nn)
{
    char *res = malloc(sizeof(char) * list_length(images) + 1);
    Node *node = images->start;
    int i = 0;
    while (node)
    {
        Block *block = img_make_block(node->data);
        if (block->label == '\0')
        {
            remove_white_margin(node->data, block);
            Img *resized = img_resize(node->data, block, 28, 28);
            // Send to the neural network
            nn_compute(nn, resized->pixels);
            res[i] = nn_getResult(nn);
        }
        else
        {
            res[i] = block->label;
        }

        node = node->next;
        i++;
    }
    res[i] = '\0';

    return res;
}

void save_res(char *res, char *filepath)
{
    FILE *file = fopen("res/res.txt", "w");
    if (file == NULL)
    {
        printf("Failed to save the result\n");
        return;
    }
    fprintf(file, "%s", res);
    fclose(file);
    printf("Result saved at: %s\n", filepath);
}

int write_dataset(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage : ./ocr write_dataset {path to source dir} {path to training data dir}\n");
        printf("Exemple : ./ocr write_dataset dataset/images/training dataset/training/set1\n");
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
    if (argc == 5 || argc == 6)
    {
        int count = atoi(argv[4]);
        int cycles = atoi(argv[3]) / count;
        NeuralNetwork *nn = create_nn(argv[2], cycles, count);
        if (argc == 6)
        {
            nn_saveBinary(nn, argv[5]);
        }
        return 0;
    }
    printf("Usage : ./ocr learn {path to dataset directory} {cycles} {training count by cycle} [saving path]\n");
    printf("Exemple : ./ocr learn dataset/training/set1 10000 10 save/Helve\n");
    return 1;
}

int read_image(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage : ./ocr read {path to neural network} {path to image}\n");
        printf("Exemple : ./ocr read save/Helve dataset/images/lorem.png\n");
        return 1;
    }
    printf("Importing image...\n");
    Img *source = img_import(argv[3]);
    printf("Segmenting image...\n");
    LinkedList *chars = segmentation(source, true);

    NeuralNetwork *nn = nn_load(argv[2]);
    char *res = send_to_cerveau(source, chars, nn);

    res = spellcheck(res);

    printf("Result: \n\n%s\n", res);
    save_res(res, "res/res.txt");
    return 0;
}

int ccl(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage: ./ocr ccl {path to nn} {path to source image}\n");
        return 1;
    }
    Img *source = img_import(argv[3]);
    Img *rotated = hough(source);
    img_save(rotated, "res/rotated.png");
    Img *new = img_import("res/rotated.png");
    img_delete(rotated);
    LinkedList *images = ccl_segmentation(new, true);

    NeuralNetwork *nn = nn_load(argv[2]);
    char *res = send_images_to_cerveau(images, nn);
    res = spellcheck(res);
    printf("Result: \n\n%s\n", res);
    save_res(res, "res/res.txt");
    return 0;
}

void hough_tmp(int argc, char **argv)
{
    if (argc != 3)
        return;
    
    Img *source = img_import(argv[2]);
    Block *tmp = img_make_block(source);
    remove_white_margin(source, tmp);
    Img *trim = img_from_block(source, tmp);
    Img *rotate = hough(trim);

    LinkedList *images = ccl_segmentation(rotate, true);
    NeuralNetwork *nn = nn_load("save/all");
    char *res = send_images_to_cerveau(images, nn);
    res = spellcheck(res);
    printf("Result: \n\n%s\n", res);
}

int main(int argc, char **argv)
{
    // Check if the res folder exists, create it otherwise
    DIR *resfile = opendir("res");
    if (resfile == NULL)
        mkdir("res", 0755);
    else
        closedir(resfile);

    if (argc == 1)
        init_window();
    else if (strcmp(argv[1], "write_dataset") == 0)
        write_dataset(argc, argv);
    else if (strcmp(argv[1], "learn") == 0)
        learn(argc, argv);
    else if (strcmp(argv[1], "read") == 0)
        read_image(argc, argv);
    else if (strcmp(argv[1], "ccl") == 0)
        ccl(argc, argv);
    else if (strcmp(argv[1], "hough") == 0)
        hough_tmp(argc, argv);
    else if (argc == 2)
        printf("Usage: ./ocr write_dataset|learn|read\n"); 
    
    return 0;
}
