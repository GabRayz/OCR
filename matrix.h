/*
    Homemaid matrix lib
*/

#ifndef MATRIX_H
# define MATRIX_H

typedef struct _Matrix {
    int width;
    int height;

    double** content;
} Matrix;

Matrix *m_init(int height, int width);
Matrix *m_init_values(int height, int width, double* content);
void m_delete(Matrix *m);
void m_print(Matrix *m);
Matrix *m_add(Matrix *a, Matrix *b);
Matrix *m_sub(Matrix *a, Matrix *b);
Matrix *m_mult(Matrix *a, Matrix *b);
Matrix *m_div(Matrix *m, double x);
Matrix *m_mult_num(Matrix *m, double x);
Matrix *m_add_num(Matrix *m, double x);
Matrix *m_sub_num(Matrix *m, double x);
Matrix *m_hadamard(Matrix *a, Matrix *b);
Matrix *m_exp(Matrix *m);
double sigmoid(double x);
Matrix *m_sigmoid(Matrix *m);
Matrix *m_sigmoid_prime(Matrix *m);
Matrix *m_transpose(Matrix *m);
float m_sum(Matrix *m);
Matrix *m_average(Matrix **list, int count);

Matrix *m_softmax_prime(Matrix *m);

#endif
