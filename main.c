#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "matrix.h"
#include "neuralnetwork.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

void read_dataset(char *files[], int dataCount)
{
    /* 
    Read the dataset/training directory to get images names
     */
    DIR *dataset;
    struct dirent *dir;
    dataset = opendir("./dataset/training/");
    if (dataset)
    {
        // For each file up to dataCount
        int i = 0;
        while ((dir = readdir(dataset)) != NULL && i < dataCount)
        {
            // Store the file name in the array
            files[i] = dir->d_name;
            i++;
        }
        closedir(dataset); 
    }
}

void dataset_to_pixels(char *files[], int dataCount) {
    for (int i = 0; i < dataCount; i++)
    {
        //printf("%s\n", files[i]);
        
        char file[100];
        strcpy(file, "dataset/training/");
        strcat(file, files[i]);
        printf("%s\n", file);

        
        MagickWand *wand = NewMagickWand();
        // MagickReadImage(wand, file);
        // if (MagickReadImage(wand, file) == MagickTrue) {
        //     printf("File open successfuly\n");
        // }else {
        //     printf("Failed to open file: %s\n", file);
        // }
        // ClearMagickWand(wand);
        DestroyMagickWand(wand);
    }
}

int main(/* int argc, char **argv */)
{
    printf("Hello World!\n");
    int sizes[] = {5, 3, 2};
    NeuralNetwork *nn = nn_init(sizes, 3);
    nn_setupRandom(nn);
    nn_delete(nn);

    // MagickWand *mw = NewMagickWand();
    // MagickReadImage(mw, "dataset/training/a_AmericanTypewriter_-1-1.bmp");
    // printf("%d\n", (int)MagickGetImageHeight(mw));

    char* files[5];
    read_dataset(files, 5);

    FILE *input;
    int get_char;
    input = fopen("dataset/training/a_AmericanTypewriter_-1-1.bmp", "r");
    // printf("%c\n", fgetc(input));
    printf("Hey\n");
    while((get_char=fgetc(input))!= EOF)
    {
        printf("%d", get_char);
    }
    fclose(input);

    //dataset_to_pixels(files, 5);

    // for (int i = 0; i < 5; i++)
    // {
    //     printf("%s\n", files[i]);
    // }
    

    return 0;
}
