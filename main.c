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

void print_image(Img *image)
{
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            printf("%d ", (int)image->pixels[y * image->width + x]);
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
            MagickAdaptiveResizeImage(mw, 28, 28);
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
        unsigned int index = rand() % 62992;
        Img *img = images[index];

        nn_compute(nn, img->pixels, (int)img->label);

        if (learn)
            nn_backProp(nn);
        //    printf("Result : %c, Label : %c, cost : %lf\n", nn_getResult(nn), img->label, nn_getCost(nn));
        results[i] = (nn_getResult(nn) == img->label) ? 1.0 : 0.0;
        if (!learn)
            printf("%c", nn_getResult(nn));
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

NeuralNetwork *create_nn()
{
    // Create a neural network, initialize it randomly, and make it learn
    int cycles = 10000;
    Img **images = read_dataset2();
    // Img **images = read_dataset(COUNT);
    printf("loaded paths\n");
    dataset_to_pixels(images, 1016 * 62);
    printf("to pixels ok !\n");
    // dataset_to_pixels(images, COUNT);

    int layerSizes[] = {784, 20, 93};
    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);
    printf("let's train\n");

    train(nn, images, cycles, 1);
    train(nn, images, cycles, 0);
    return nn;
}

NeuralNetwork *create_nn_from_img(Img **images)
{
    // Create a neural network, initialize it randomly, and make it learn
    int cycles = 10000;

    int layerSizes[] = {784, 25, 93};
    NeuralNetwork *nn = nn_init(layerSizes, 3);
    nn_setupRandom(nn);

    train(nn, images, cycles, 1);
    train(nn, images, cycles, 0);
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

Img **images_from_list(Img *source, LinkedList *chars, char *label)
{
    int length = list_length(chars);
    Img **images = malloc(sizeof(Img) * length);
    Node *n = chars->start;
    // printf("%d\n", strlen(label));
    int i = 0;
    for (i = 0; i < length && label[i] != '\0' && i < 1000; i++)
    {
        Img *c = img_resize(source, n->block, 28, 28);
        char truc[] = {i / 100 + 48, (i / 10) % 10 + 48, i % 10 + 48, '\0'};
        img_save(c->pixels, c->width, c->height, concat(concat("res/", truc), ".png"));
        images[i] = c;
        images[i]->label = label[i];
        n = n->next;
    }
    // printf("%d\n", i);
    return images;
}

int main()
{
    init_window();
    return 0;
    // read_dataset2();
    MagickWandGenesis();
    Img *source = img_import("dataset/images/paragraphes.jpeg");
    LinkedList *chars = segmentation(source);

    Block *i = list_get_index(chars, 16);
    
    Img *truc = img_from_block(source, i);
    //img_print(truc->pixels, truc->width, truc->height);
    img_save(truc->pixels, truc->width, truc->height, "res.png");
    // char *string = "LeLoremIpsumestsimplementdufauxtexteemployédanslacompositionetlamiseenpageavantimpression.LeLoremIpsumestlefauxtextestandarddel'imprimeriedepuislesannées1500,quandunimprimeuranonymeassemblaensembledesmorceauxdetextepourréaliserunlivrespécimendepolicesdetexte.Iln'apasfaitquesurvivrecinqsiècles,maiss'estaussiadaptéàlabureautiqueinformatique,sansquesoncontenun'ensoitmodifié.Ilaétépopularisédanslesannées1960grâceàlaventedefeuillesLetrasetcontenantdespassagesduLoremIpsum,et,plusrécemment,parsoninclusiondansdesapplicationsdemiseenpagedetexte,commeAldusPageMaker.Onsaitdepuislongtempsquetravailleravecdutextelisibleetcontenantdusensestsourcededistractions,etempêchedeseconcentrersurlamiseenpageelle-même.L'avantageduLoremIpsumsuruntextegénériquecomme'Dutexte.Dutexte.Dutexte.'estqu'ilpossèdeunedistributiondelettresplusoumoinsnormale,etentoutcascomparableaveccelledufrançaisstandard.DenombreusessuiteslogiciellesdemiseenpageouéditeursdesitesWebontfaitduLoremIpsumleurfauxtextepardéfaut,etunerecherchepour'LoremIpsum'vousconduiraversdenombreuxsitesquin'ensontencorequ'àleurphasedeconstruction.Plusieursversionssontapparuesavecletemps,parfoisparaccident,souventintentionnellement(histoired'yrajouterdepetitsclinsd'oeil,voiredesphrasesembarassantes).";
    // Img **images = images_from_list(source, chars, string);

    // NeuralNetwork *nn = create_nn_from_img(images);

    //print_image(images[0]);
    // printf("%c\n", images[0]->label);
    // NeuralNetwork *nn = create_nn();
    // Img *m = img_from_block(source, chars->start->next->next->next->next->next->next->block);
    // remove_white_margin(source, chars->start->next->next->next->next->next->next->block);
    // Img *m = img_resize(source, chars->start->next->next->next->next->next->next->block, 28, 28);
    // img_save(m->pixels, m->width, m->height, "res.png");

    // print_image(m);
    // nn_compute(nn, m, 'm');
    // printf("%c\n", nn_getResult(nn));
    // char *res = send_to_cerveau(source, chars, nn);
    // printf("%s\n", res);

    return 0;
}
