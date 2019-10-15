#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include <assert.h>

/* 
    Matrices declaration and operations
    struct matrix:
        width: The width of the matrix
        height: The height of the matrix
        content: The content of the matrix, represented by an array of arrays of floats
*/

Matrix *m_init(int height, int width)
{
    assert(height > 0 && width > 0);

    // Allocate the right size
    Matrix *m = malloc(sizeof(Matrix));
    m->height = height;
    m->width = width;
    m->content = malloc(height * sizeof(double *)); // Size of an array of float, height times

    // Create a single big buffer to reduce the number of system calls
    double* buffer = calloc(width * height, sizeof(double));

    for (int y = 0; y < height; y++)
    {
        // The pointer is shifted by width for each iteration
        m->content[y] = buffer + y * width;
    }

    return m;
}

Matrix *m_init_values(int height, int width, double *content)
{
    Matrix *m = m_init(height, width); // Create an empty matrix

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            m->content[y][x] = content[y * width + x];
        }
    }

    return m;
}

void m_delete(Matrix *m)
{
    if (m == NULL)
        return;

    free(m->content[0]); // Free whole buffer
    free(m->content); // Free the array of array
    free(m);          // Free the struct
}

void m_print(Matrix *m)
{
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            printf("%f ", m->content[y][x]);
        }
        printf("\n");
    }
}

Matrix *m_add(Matrix *a, Matrix *b)
{
    if (a->width != b->width || a->height != b->height)
    {
        printf("Matrix addiction: Sizes are not valid\n");
        exit(1);
    }

    Matrix *res = m_init(a->height, a->width);

    for (int y = 0; y < res->height; y++)
    {
        for (int x = 0; x < res->width; x++)
        {
            res->content[y][x] = a->content[y][x] + b->content[y][x];
        }
    }

    return res;
}

Matrix *m_sub(Matrix *a, Matrix *b)
{
    if (a->width != b->width || a->height != b->height)
    {
        printf("Matrix substraction: Sizes are not valid\n");
        exit(1);
    }

    Matrix *res = m_init(a->height, a->width);

    for (int y = 0; y < res->height; y++)
    {
        for (int x = 0; x < res->width; x++)
        {
            res->content[y][x] = a->content[y][x] - b->content[y][x];
        }
    }

    return res;
}

Matrix *m_mult(Matrix *a, Matrix *b)
{
    if (a->width != b->height)
    {
        printf("Matrix product: Sizes are not valid\n");
        exit(1);
    }

    Matrix *res = m_init(a->height, b->width);

    for (int y = 0; y < a->height; y++)
    {
        for (int x = 0; x < b->width; x++)
        {
            for (int k = 0; k < a->width; k++)
            {
                res->content[y][x] += a->content[y][k] * b->content[k][x];
            }
        }
    }

    return res;
}

Matrix *m_add_num(Matrix *m, double value)
{
    Matrix *res = m_init(m->height, m->width);

    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            res->content[y][x] = m->content[y][x] + value;
        }
    }

    return res;
}

Matrix *m_sub_num(Matrix *m, double value)
{
    return m_add_num(m, -value);
}

Matrix *m_mult_num(Matrix *m, double value)
{
    Matrix *res = m_init(m->height, m->width);

    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            res->content[y][x] = m->content[y][x] * value;
        }
    }

    return res;
}

Matrix *m_div(Matrix *m, double value)
{
    return m_mult_num(m, 1 / value);
}

/* 
    Useful matrices operations in backpropagation computations
*/

Matrix *m_hadamard(Matrix *a, Matrix *b)
{
    if (a->width != b->width || a->height != b->height)
    {
        printf("m_hadamard: Sizes not valid.\n");
        exit(1);
    }

    Matrix *res = m_init(a->height, a->width);

    for (int y = 0; y < res->height; y++)
    {
        for (int x = 0; x < res->width; x++)
        {
            res->content[y][x] = a->content[y][x] * b->content[y][x];
        }
    }

    return res;
}

Matrix *m_exp(Matrix *m)
{
    Matrix *res = m_init(m->height, m->width);

    for (int y = 0; y < res->height; y++)
    {
        for (int x = 0; x < res->width; x++)
        {
            res->content[y][x] = exp(m->content[y][x]);
        }
    }

    return res;
}

double sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}

Matrix *m_sigmoid(Matrix *m)
{
    Matrix *res = m_init(m->height, m->width);
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            res->content[y][x] = sigmoid(m->content[y][x]);
        }
    }
    return res;
}

Matrix *m_sigmoid_prime(Matrix *m)
{
    Matrix *res = m_init(m->height, m->width);
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            double sig = sigmoid(res->content[y][x]);
            res->content[y][x] = sig * (1 - sig); // Just compute the derivative of sigmoid
        }
    }
    return res;
}

Matrix *m_softmax_prime(Matrix *m)
{
    Matrix *res = m_init(m->height, m->width);
    Matrix *exp = m_exp(m);
    double sum = m_sum(exp);
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            res->content[y][x] = exp->content[y][x] * (sum - exp->content[y][x]);
            // double sig = sigmoid(res->content[y][x]);
            // res->content[y][x] = sig * (1 - sig); // Just compute the derivative of sigmoid
        }
    }
    return res;
}

Matrix *m_transpose(Matrix *m)
{
    // Transpose rows and colomns

    Matrix *res = m_init(m->width, m->height);

    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            res->content[x][y] = m->content[y][x];
        }
    }
    return res;
}

float m_sum(Matrix *m)
{
    // Make the sum of all the matrix values
    float res = 0;
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            res += m->content[y][x];
        }
    }

    return res;
}

Matrix *m_average(Matrix **list, int count)
{
    if (count < 1)
    {
        printf("Matrix average: List must not be empty\n");
        exit(1);
    }
    int height = list[0]->height;
    int width = list[0]->width;
    for (int i = 0; i < count; i++)
    {
        if (list[i]->height != height || list[i]->width != width)
        {
            printf("Matrix average: All matrices of the list must have the same size");
            exit(2);
        }
    }

    Matrix *res = m_init(list[0]->height, list[0]->width);

    for (int y = 0; y < list[0]->height; y++)
    {
        for (int x = 0; x < list[0]->width; x++)
        {
            for (int i = 0; i < count; i++)
            {
                res->content[y][x] += list[i]->content[y][x];
            }
            res->content[y][x] /= count;
        }
    }
    return res;
}
