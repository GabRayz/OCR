#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

Matrix *m_init(int height, int width)
{
    Matrix *m = malloc(sizeof(Matrix));
    m->height = height;
    m->width = width;
    m->content = malloc(sizeof(double *) * height);

    for (int y = 0; y < height; y++)
    {
        m->content[y] = malloc(sizeof(double) * width);
        for (int x = 0; x < width; x++)
        {
            m->content[y][x] = 0;
        }
    }

    return m;
}

Matrix *m_init_values(int height, int width, double *content)
{
    Matrix *m = m_init(height, width);

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
    for (int y = 0; y < m->height; y++)
    {
        free(m->content[y]);
    }

    free(m->content);
    free(m);
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
        printf("PAS BIEN SALO\n");
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
        printf("PAS BIEN SALO\n");
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
        printf("PAS BIEN SALO\n");
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
            res->content[y][x] = sigmoid(res->content[y][x]);
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
            res->content[y][x] = sig * (1 - sig);
        }
    }
    return res;
}

Matrix *m_tanspose(Matrix *m)
{
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
    // TODO: Vérifier les dimensions des matrices
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