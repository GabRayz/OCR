#ifndef MAIN_H
#define MAIN_H

#include "neuralnetwork.h"
#include "segmentation.h"

void train(NeuralNetwork *nn, Img **images, int images_count, int cycles, int learn);
NeuralNetwork *create_nn();
NeuralNetwork *create_nn_from_img(Img **images, int images_count);
LinkedList *segmentation(Img *source);
char *send_to_cerveau(Img *source, LinkedList *chars, NeuralNetwork *nn);

#endif
