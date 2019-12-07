#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "neuralnetwork.h"
#include "image.h"
#include <assert.h>

NeuralNetwork *nn_init(int *layerSizes, int layerCount)
{
    NeuralNetwork *nn = malloc(sizeof(NeuralNetwork));
    nn->layerCount = layerCount;
    nn->layerSizes = layerSizes;

    // Allocate the size of the Matrix arrays. One matrix per layer.
    nn->biaises = malloc(sizeof(Matrix *) * layerCount);
    nn->weights = malloc(sizeof(Matrix *) * layerCount);
    nn->activations = malloc(sizeof(Matrix *) * layerCount);
    nn->z = malloc(sizeof(Matrix *) * layerCount);
    nn->errors = malloc(sizeof(Matrix *) * layerCount);
    nn->dBiaises = malloc(sizeof(Matrix *) * layerCount);
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
            nn->dBiaises[l] = m_init(layerSizes[l], 1);
            nn->dWeights[l] = m_init(layerSizes[l], layerSizes[l - 1]);
            nn->z[l] = m_init(layerSizes[l], 1);
            nn->errors[l] = m_init(layerSizes[l], 1);
            nn->weights[l] = m_init(layerSizes[l], layerSizes[l - 1]);
        }
        else
        {
            nn->biaises[l] = NULL;
            nn->dBiaises[l] = NULL;
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
            m_delete(nn->dBiaises[l]);
            m_delete(nn->z[l]);
            m_delete(nn->errors[l]);
            m_delete(nn->weights[l]);
        }
    }

    // Delete all lists
    free(nn->biaises);
    free(nn->dBiaises);
    free(nn->weights);
    free(nn->activations);
    free(nn->z);
    free(nn->errors);

    free(nn->y);
    free(nn); // Delete the struct
}

/**
 * Sets all parameters randomly using a Gaussian distribution
 */
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

/**
 * Sets the values of the first layer from an image to process it later.
 */
void nn_initFirstLayer(NeuralNetwork *nn, double *pixels)
{
    // Set each neuron's value according to the pixels
    for (int j = 0; j < nn->activations[0]->height; j++)
    {
        nn->activations[0]->content[j][0] = pixels[j];
    }
}

/**
 * Process an image.
 */
void nn_compute(NeuralNetwork *nn, double *pixels)
{
    /* 
    params:
        nn: Pointer to the Neural Network
        pixels: Pointer to the array of doubles representing the values of image's pixels

    action:
        Init the first layer (nn_initFirstLayer)
        Set matrix y
        Compute the activations on each layer (nn_feedForward)
    */
    nn_initFirstLayer(nn, pixels);

    nn_feedForward(nn);
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
        if (l == nn->layerCount - 1 && 0)
        {
            // Compute a softmax for the last layer
            // Softmax formula : Aj = exp(Zj) /  Sum_over_k(exp(Zk))
            Matrix *exp = m_exp(nn->z[l]);
            double sum = m_sum(exp);
            nn->activations[l] = m_div(exp, sum);
            //printf("%lf\n", m_sum(nn->activations[l]));
            m_delete(exp);
        }
        else
        {
            nn->activations[l] = m_sigmoid(nn->z[l]);
        }

        // Free the temporary matrix
        m_delete(product);
    }
}

void nn_backProp(NeuralNetwork *nn, char label, int apply, int count)
{
    assert(label >= 33);

    m_delete(nn->y);

    // Set the matrix of expected results
    nn->y = m_init(nn->activations[nn->layerCount - 1]->height, 1);
    nn->y->content[label - 33][0] = 1.0;

    // Compute the errors and each layer, then compute the gradient's component

    // Free previous errors
    for (int l = 1; l < nn->layerCount; l++)
    {
        m_delete(nn->errors[l]);
        // m_delete(nn->dWeights[l]);
    }
    // Compute the error on the output layer
    Matrix *sum = m_sub(nn->activations[nn->layerCount - 1], nn->y);
    Matrix *costPrime = m_mult_num(sum, 1.0 / count);
    Matrix *zSigmoidPrime = m_sigmoid_prime(nn->z[nn->layerCount - 1]);
    // Matrix *zSigmoidPrime = m_softmax_prime(nn->z[nn->layerCount - 1]);
    nn->errors[nn->layerCount - 1] = m_hadamard(costPrime, zSigmoidPrime);
    // Free temporary matrices
    m_delete(costPrime);
    m_delete(zSigmoidPrime);
    m_delete(sum);

    for (int l = nn->layerCount - 2; l > 0; l--)
    {
        // Compute intermediate matrices
        Matrix *transposed = m_transpose(nn->weights[l + 1]);
        Matrix *product = m_mult(transposed, nn->errors[l + 1]);
        m_delete(transposed);
        zSigmoidPrime = m_sigmoid_prime(nn->z[l]);

        // Compute the error
        nn->errors[l] = m_hadamard(product, zSigmoidPrime);
        m_delete(product);
        m_delete(zSigmoidPrime);
    }
    // Compute the gradient's components
    for (int l = 1; l < nn->layerCount; l++)
    {
        // nn->dWeights[l] = m_init(nn->weights[l]->height, nn->weights[l]->width);
        Matrix *oldDBiaises = nn->dBiaises[l];
        nn->dBiaises[l] = m_add(nn->dBiaises[l], nn->errors[l]);
        m_delete(oldDBiaises);
        for (int j = 0; j < nn->weights[l]->height; j++)
        {
            for (int i = 0; i < nn->weights[l]->width; i++)
            {
                // dC/dW = activation(previous layer) * error(current layer
                nn->dWeights[l]->content[j][i] += nn->activations[l - 1]->content[i][0] * nn->errors[l]->content[j][0];
            }
        }
    }

    if (apply)
    {
        // m_print(nn->dBiaises[1]);
        // Apply modifiers
        for (int l = 1; l < nn->layerCount; l++)
        {
            Matrix *oldBiaises = nn->biaises[l];
            Matrix *oldWeights = nn->weights[l];
            // Apply learning rate
            Matrix *biaisDelta = m_mult_num(nn->dBiaises[l], 0.25);
            Matrix *weightsDelta = m_mult_num(nn->dWeights[l], 0.25);
            nn->biaises[l] = m_sub(nn->biaises[l], biaisDelta);
            nn->weights[l] = m_sub(nn->weights[l], weightsDelta);
            m_delete(biaisDelta);
            m_delete(weightsDelta);
            m_delete(oldBiaises);
            m_delete(oldWeights);
            m_delete(nn->dBiaises[l]);
            m_delete(nn->dWeights[l]);
            nn->dBiaises[l] = m_init(nn->layerSizes[l], 1);
            nn->dWeights[l] = m_init(nn->layerSizes[l], nn->layerSizes[l - 1]);
        }
    }
}

double GaussianRand()
{
    double r1 = 1.0 - (double)rand() / RAND_MAX;
    double r2 = 1.0 - (double)rand() / RAND_MAX;

    double randStdNormal = sqrt(-2.0 * log(r1)) * sin(2.0 * M_PI * r2);
    return randStdNormal;
}

void nn_saveBinary(NeuralNetwork *nn, char *filepath)
{
    printf("Saving in binary mode...\n");
    FILE *file = fopen(filepath, "wb");
    if (file == NULL)
    {
        printf("Error while saving the network\n");
        exit(1);
    }

    // Write number of layer
    fwrite(&nn->layerCount, sizeof(int), 1, file);

    // Write layer sizes
    for (int i = 0; i < nn->layerCount; i++)
    {
        fwrite(&nn->layerSizes[i], sizeof(int), 1, file);
    }
    // Write biaises
    for (int l = 1; l < nn->layerCount; l++)
    {
        for (int n = 0; n < nn->activations[l]->height; n++)
        {
            fwrite(&nn->biaises[l]->content[n][0], sizeof(double), 1, file);
        }
    }

    // Write weights
    for (int l = 1; l < nn->layerCount; l++)
    {
        for (int y = 0; y < nn->weights[l]->height; y++)
        {
            for (int x = 0; x < nn->weights[l]->width; x++)
            {
                fwrite(&nn->weights[l]->content[y][x], sizeof(double), 1, file);
            }

            // fwrite(&nn->weights[l]->content[y], sizeof(double)*nn->weights[l]->width, 1, file);
        }
    }

    printf("Neural network saved at : %s\n", filepath);

    fclose(file);
    return;
}

/**
 * Loads a saved neural network from the filepath.
 */
NeuralNetwork *nn_load(char *filepath)
{
    printf("Loading the network...\n");
    FILE *file = fopen(filepath, "rb");
    if (file == NULL)
    {
        printf("Error while loading the network\n");
        exit(1);
    }
    // Reading layer count
    int layerCount;
    fread(&layerCount, sizeof(int), 1, file);

    int *layerSizes = malloc(sizeof(int) * layerCount);
    fread(layerSizes, sizeof(int) * layerCount, 1, file);

    printf("Creating the neural network...\n");
    NeuralNetwork *nn = nn_init(layerSizes, layerCount);

    for (int l = 1; l < layerCount; l++)
    {
        for (int y = 0; y < nn->biaises[l]->height; y++)
        {
            fread(nn->biaises[l]->content[y], sizeof(double), 1, file);
        }
    }

    for (int l = 1; l < layerCount; l++)
    {
        for (int y = 0; y < nn->weights[l]->height; y++)
        {
            fread(nn->weights[l]->content[y], sizeof(double), nn->weights[l]->width, file);
        }
    }

    printf("Neural network loaded !\n");
    fclose(file);
    return nn;
}

/**
 * Train a neural network from a set of images.
 */
void train(NeuralNetwork *nn, Img **images, int images_count, int cycles, int count)
{
    /* 
    Train the neural network with the given set of images
    */
    if (cycles == 0)
        return;
    printf("Training...\n");
    fputs("\e[?25l", stdout); /* hide the cursor */
    double sum = 0;
    double accuracy = 0;
    for (int i = 0; i < cycles * count; i++)
    {
        if (i % 1000 == 0)
        {
            // Stop training when accuracy is 100 or
            double tmp = (sum / 1000) * 100;
            // if (tmp < accuracy || tmp == 100)
            //     break;
            accuracy = tmp;
            sum = 0;
        }
        printf("\r%d / %d, accuracy = %lf", i + 1, cycles * count, accuracy);
        unsigned int index = rand() % images_count;
        Img *img = images[index];
        // print_image(img);
        // printf("%c %d\n", img->label, img->label);

        nn_compute(nn, img->pixels);
        nn_backProp(nn, (int)img->label, i % count == 0, count);

        sum += (nn_getResult(nn) == img->label) ? 1.0 : 0.0;
    }
    fputs("\e[?25h", stdout); /* show the cursor */
    printf("\n");
}
