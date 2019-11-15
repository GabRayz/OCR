#ifndef NEURALNETWORK_H
# define NEURALNETWORK_H

#include "matrix.h"
#include "image.h"

/**
 * Structure representing a Neural Network.
 * A neural network contains several arrays of matrix to represent the network.
 * For instance, the Matrix array 'biaises' reprensents all the biaises of the network, organised by layer via several Matrix.
 * The first matrix of the array contains the biais of each neuron of the first layers.
 * Call nn_init() to instantiate a network.
 */
typedef struct _NeuralNetwork {
    int layerCount;
    /**
     * Array of int of size this.layerCount
     * Each index corresponds to the number of neurons in the corresponding layer
     */
    int* layerSizes;

    Matrix** biaises; // All biaises, each layer L being represented by a Matrix at the Lth element of the array.
    Matrix** weights;
    Matrix** activations; // a = Sigmoid(z)
    /**
     * Used to store the weighed input of the layer, before applying an activation function.
     * z = wa + b
    */
    Matrix** z;
    /**
     * Y is the matrix of expected result of the output layer.
     */
    Matrix* y;

    Matrix** errors;
    Matrix** dWeights;
    Matrix** dBiaises;

} NeuralNetwork;

NeuralNetwork *nn_init(int *layerSizes, int layerCount);
void nn_delete(NeuralNetwork *nn);

void nn_compute(NeuralNetwork *nn, double *pixels);
void nn_setupRandom(NeuralNetwork *nn);
void nn_initFirstLayer(NeuralNetwork *nn, double *pixels);
void nn_feedForward(NeuralNetwork *nn);
void nn_backProp(NeuralNetwork *nn, char label, int apply, int count);
double GaussianRand();
char nn_getResult(NeuralNetwork *nn);
double nn_getCost(NeuralNetwork *nn);
void nn_saveBinary(NeuralNetwork *nn, char *filepath);
NeuralNetwork *nn_load(char *filepath);
void train(NeuralNetwork *nn, Img **images, int images_count, int cycles, int count);

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#endif
