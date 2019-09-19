#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "neuralnetwork.h"

NeuralNetwork *nn_init(int *layerSizes, int layerCount)
{
    NeuralNetwork *nn = malloc(sizeof(NeuralNetwork));
    nn->layerCount = layerCount;

    // Allocate the size of the Matrix arrays. One matrix per layer.
    nn->biaises = malloc(sizeof(Matrix *) * layerCount);
    nn->weights = malloc(sizeof(Matrix *) * layerCount);
    nn->activations = malloc(sizeof(Matrix *) * layerCount);
    nn->z = malloc(sizeof(Matrix *) * layerCount);
    nn->errors = malloc(sizeof(Matrix *) * layerCount);
    // Allocate the size of a Matrix for the expected result of the last layer.
    nn->y = m_init(layerSizes[layerCount - 1], 1);

    // Go through all layers to initialize the matrices.
    for (int l = 0; l < layerCount; l++)
    {
        nn->activations[l] = m_init(layerSizes[l], 1);
        if (l != 0) // Initializing biaises, weights and errors for the first layer is useless
        {
            nn->biaises[l] = m_init(layerSizes[l], 1);
            nn->z[l] = m_init(layerSizes[l], 1);
            nn->errors[l] = m_init(layerSizes[l], 1);
            nn->weights[l] = m_init(layerSizes[l], layerSizes[l - 1]);
        }
        else
        {
            nn->biaises[l] = NULL;
            nn->z[l] = NULL;
            nn->errors[l] = NULL;
            nn->weights[l] = NULL;
        }
    }

    return nn;
}

void nn_delete(NeuralNetwork *nn)
{
    for (int l = 0; l < nn->layerCount; l++)
    {
        // Delete all matrices of the lists
        m_delete(nn->activations[l]);
        if (l != 0)
        {
            m_delete(nn->biaises[l]);
            m_delete(nn->z[l]);
            m_delete(nn->errors[l]);
            m_delete(nn->weights[l]);
        }
    }

    // Delete all lists
    free(nn->biaises);
    free(nn->weights);
    free(nn->activations);
    free(nn->z);
    free(nn->errors);

    free(nn->y);
    free(nn); // Delete the struct
}

void nn_setupRandom(NeuralNetwork *nn)
{
    // Initialize each biais and weights with random values
    for (int l = 1; l < nn->layerCount; l++)
    {
        for (int i = 0; i < nn->biaises[l]->height; i++)
        {
            nn->biaises[l]->content[i][0] = GaussianRand();

            for (int j = 0; j < nn->weights[l]->width; j++)
            {
                nn->weights[l]->content[i][j] = GaussianRand();
            }
        }
    }
    return 0;
}

void nn_initFirstLayer(NeuralNetwork *nn, double *pixels)
{
    // Set each neuron's value according to the pixels
}

void nn_compute(NeuralNetwork *nn, double *pixels, int label)
{
    /* 
    params:
        nn: Pointer to the Neural Network
        pixels: Pointer to the array of doubles representing the values of image's pixels
        label: The id of the expected output neuron

    action:
        Init the first layer (nn_initFirstLayer)
        Set matrix y
        Compute the activations on each layer (nn_feedForward)
    */
}

void nn_feedForward(NeuralNetwork *nn)
{
    // Compute the activation on each layer
}

void nn_backProp(NeuralNetwork *nn)
{
    // Compute the errors and each layer, then compute the gradient's component
}

double GaussianRand()
{
    double r1 = 1.0 - (double)rand() / RAND_MAX;
    double r2 = 1.0 - (double)rand() / RAND_MAX;

    double randStdNormal = sqrt(-2.0 * log(r1)) * sin(2.0 * M_PI * r2);
    return randStdNormal;
}
