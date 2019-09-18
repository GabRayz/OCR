#include <stdlib.h>
#include <stdio.h>
#include "neuralnetwork.h"

NeuralNetwork *nn_init(int *layerSizes, int layerCount)
{
    NeuralNetwork *nn = malloc(sizeof(NeuralNetwork));
    nn->layerCount = layerCount;
    nn->biaises = malloc(sizeof(Matrix *) * layerCount);
    nn->weights = malloc(sizeof(Matrix *) * layerCount);
    nn->activations = malloc(sizeof(Matrix *) * layerCount);
    nn->z = malloc(sizeof(Matrix *) * layerCount);
    nn->errors = malloc(sizeof(Matrix *) * layerCount);

    nn->y = m_init(layerSizes[layerCount - 1], 1);
    
    for (int l = 0; l < layerCount; l++)
    {
        nn->activations[l] = m_init(layerSizes[l], 1);
        if (l != 0)
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
        m_delete(nn->activations[l]);
        if (l != 0)
        {
            m_delete(nn->biaises[l]);
            m_delete(nn->z[l]);
            m_delete(nn->errors[l]);
            m_delete(nn->weights[l]);
        }
    }
    
    free(nn->biaises);
    free(nn->weights);
    free(nn->activations);
    free(nn->z);
    free(nn->errors);

    free(nn->y);
    free(nn);
}
