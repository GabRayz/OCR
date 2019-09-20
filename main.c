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

int main(/* int argc, char **argv */)
{
    printf("Hello World!\n");
    int sizes[] = {5, 3, 2};
    NeuralNetwork *nn = nn_init(sizes, 3);
    nn_setupRandom(nn);
    nn_delete(nn);

    // MagickWand *mw = NewMagickWand();

    //char files[2][14];
    // char files[2][14] = *read_dataset(5);
    char* files[5];
    read_dataset(files, 5);
    for (int i = 0; i < 5; i++)
    {
        printf("%s\n", files[i]);
    }
    

    return 0;
}
