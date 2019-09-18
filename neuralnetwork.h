#include "matrix.h"

typedef struct _NeuralNetwork {
    int layerCount;
    int* layerSizes;

    Matrix** biaises;
    Matrix** weights;
    Matrix** activations;
    Matrix** z;
    Matrix* y;

    Matrix** errors;

} NeuralNetwork;

NeuralNetwork *nn_init(int *layerSizes, int layerCount);
void nn_delete(NeuralNetwork *nn);
