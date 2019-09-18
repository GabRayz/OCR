#include <stdio.h>
#include "matrix.h"
#include "neuralnetwork.h"

int main(int argc, char **argv)
{
    printf("Hello World!\n");
    int sizes[] = {5, 3, 2};
    NeuralNetwork *nn = nn_init(sizes, 3);
    nn_delete(nn);
    return 0;
}
