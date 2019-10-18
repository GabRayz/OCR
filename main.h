#ifndef MAIN_H
#define MAIN_H

#include "neuralnetwork.h"
#include "segmentation.h"

NeuralNetwork *create_nn();
LinkedList *segmentation(Img *source);
char *send_to_cerveau(Img *source, LinkedList *chars, NeuralNetwork *nn);

#endif
