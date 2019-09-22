#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
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
    nn->dWeights = malloc(sizeof(Matrix *) * layerCount);
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
            nn->dWeights[l] = m_init(layerSizes[l], layerSizes[l - 1]);
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
}

void nn_initFirstLayer(NeuralNetwork *nn, double *pixels)
{
    // Set each neuron's value according to the pixels
    for (int j = 0; j < nn->activations[0]->height; j++)
    {
        nn->activations[0]->content[j][0] = pixels[j];
    }
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

    nn_initFirstLayer(nn, pixels);
    m_delete(nn->y);

    // Set the matrix of expected results
    nn->y = m_init(nn->activations[nn->layerCount - 1]->height, 1);
    nn->y->content[label - 33][0] = 1.0;

    nn_feedForward(nn);
    // nn_backProp(nn);
}

char nn_getResult(NeuralNetwork *nn)
{
    /* Return the output of the network */

    // Get the maximum activation
    int max_index = 0;
    double max_value = 0;
    Matrix *result = nn->activations[nn->layerCount - 1];
    for (int k = 0; k < result->height; k++)
    {
        if (result->content[k][0] > max_value)
        {
            max_value = result->content[k][0];
            max_index = k;
        }
    }

    // Cast the index of the neuron to the corresponding character
    return max_index + 33;
}

double nn_getCost(NeuralNetwork *nn)
{
    // Compute a - y
    Matrix *sub = m_sub(nn->activations[nn->layerCount - 1], nn->y);
    // Compute (a - y)^2
    Matrix *hadamard = m_hadamard(sub, sub);
    // Compute the sum
    double cost = m_sum(hadamard);

    m_delete(sub);
    m_delete(hadamard);
    return cost;
}

void nn_feedForward(NeuralNetwork *nn)
{
    // Compute the activation on each layer
    for (int l = 1; l < nn->layerCount; l++)
    {
        // Free z and activation
        m_delete(nn->z[l]);
        m_delete(nn->activations[l]);

        // Store the product m*a in a temporary matrix
        Matrix *product = m_mult(nn->weights[l], nn->activations[l - 1]);
        nn->z[l] = m_add(product, nn->biaises[l]);
        nn->activations[l] = m_sigmoid(nn->z[l]);

        // Free the temporary matrix
        m_delete(product);
    }
}

void nn_backProp(NeuralNetwork *nn)
{
    // Compute the errors and each layer, then compute the gradient's component

    // Free previous errors
    for (int l = 1; l < nn->layerCount; l++)
    {
        m_delete(nn->errors[l]);
        m_delete(nn->dWeights[l]);
        nn->dWeights[l] = m_init(nn->weights[l]->height, nn->weights[l]->width);
    }

    // Compute the error on the output layer
    Matrix *costPrime = m_sub(nn->activations[nn->layerCount - 1], nn->y);
    Matrix *zSigmoidPrime = m_sigmoid_prime(nn->z[nn->layerCount - 1]);
    nn->errors[nn->layerCount - 1] = m_hadamard(costPrime, zSigmoidPrime);

    // Free temporary matrices
    m_delete(costPrime);
    m_delete(zSigmoidPrime);

    for (int l = nn->layerCount - 2; l > 0; l--)
    {
        // Compute intermediate matrices
        Matrix *transposed = m_transpose(nn->weights[l + 1]);
        Matrix *product = m_mult(transposed, nn->errors[l + 1]);
        m_delete(transposed);
        Matrix *zSigmoidPrime = m_sigmoid_prime(nn->z[l]);

        // Compute the error
        nn->errors[l] = m_hadamard(product, zSigmoidPrime);
        m_delete(product);
        m_delete(zSigmoidPrime);
    }

    // Compute the gradient's components
    for (int l = 1; l < nn->layerCount; l++)
    {
        // nn->dBiaises[l] = nn->errors
        for (int j = 0; j < nn->weights[l]->height; j++)
        {
            for (int i = 0; i < nn->weights[l]->width; i++)
            {
                // dC/dW = activation(previous layer) * error(current layer
                nn->dWeights[l]->content[j][i] = nn->activations[l - 1]->content[i][0] * nn->errors[l]->content[j][0];
            }
        }
    }

    // Apply modifiers
    for (int l = 1; l < nn->layerCount; l++)
    {
        nn->biaises[l] = m_sub(nn->biaises[l], nn->errors[l]);
        nn->weights[l] = m_sub(nn->weights[l], nn->dWeights[l]);
    }
}

double GaussianRand()
{
    double r1 = 1.0 - (double)rand() / RAND_MAX;
    double r2 = 1.0 - (double)rand() / RAND_MAX;

    double randStdNormal = sqrt(-2.0 * log(r1)) * sin(2.0 * M_PI * r2);
    return randStdNormal;
}
