#include <stdio.h>
#include "matrix.h"
#include "neuralnetwork.h"
#include <ImageMagick-7/MagickWand/MagickWand.h>

int main(/* int argc, char **argv */)
{
    printf("Hello World!\n");
    int sizes[] = {5, 3, 2};
    NeuralNetwork *nn = nn_init(sizes, 3);
    nn_setupRandom(nn);
    nn_delete(nn);

    MagickWand *mw = NewMagickWand();
    
    return 0;
}
