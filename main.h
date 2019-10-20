#ifndef MAIN_H
#define MAIN_H

#include "neuralnetwork.h"
#include "segmentation.h"

NeuralNetwork *create_nn(char *filepath, int cycles);
char *send_to_cerveau(Img *source, LinkedList *chars, NeuralNetwork *nn);
void save_res(char *res, char *filepath);
int write_dataset(int argc, char **argv);
int learn(int argc, char **argv);
int read_image(int argc, char **argv);

#endif
