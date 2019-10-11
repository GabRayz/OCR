#ifndef DATASET_H
#define DATASET_H

#include "image.h"

Img **read_dataset(int dataCount);
Img **read_dataset2();
void dataset_to_pixels(Img **images, int dataCount);
char *concat(char *a, char *b);

#endif
