#ifndef NEURALNETWORK_H
# define NEURALNETWORK_H

#include "matrix.h"

typedef struct _NeuralNetwork {
    int layerCount;
    int* layerSizes;

    /* 
    A neural network contains several arrays of matrix to represent the network.
    For instance, the Matrix array 'biaises' reprensents all the biaises of the network, organised by layer via several Matrix.
    The first matrix of the array contains the biais of each neuron of the first layers.
     */

    Matrix** biaises; // All biaises, each layer L being represented by a Matrix at the Lth element of the array.
    Matrix** weights;
    Matrix** activations; // a = Sigmoid(z)
    Matrix** z; // z = wa + b
    Matrix* y; // Y is the matrix of expected result of the output layer.

    Matrix** errors;
    Matrix** dWeights;

} NeuralNetwork;

NeuralNetwork *nn_init(int *layerSizes, int layerCount);
void nn_delete(NeuralNetwork *nn);

void nn_compute(NeuralNetwork *nn, double *pixels, unsigned char label);
void nn_setupRandom(NeuralNetwork *nn);
void nn_initFirstLayer(NeuralNetwork *nn, double *pixels);
void nn_feedForward(NeuralNetwork *nn);
void nn_backProp(NeuralNetwork *nn);
double GaussianRand();
char nn_getResult(NeuralNetwork *nn);
double nn_getCost(NeuralNetwork *nn);
void nn_save(NeuralNetwork *nn, char *filepath);
void nn_saveBinary(NeuralNetwork *nn, char *filepath);
NeuralNetwork *nn_load(char *filepath);

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#endif
