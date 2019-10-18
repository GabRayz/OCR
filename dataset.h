#ifndef DATASET_H
#define DATASET_H

#include "image.h"

Img **read_dataset(int dataCount);
Img **read_dataset2();
void dataset_to_pixels(Img **images, int dataCount);
void create_dataset_from_img(char *dirpath, Img **images, int dataCount, int index);
char *concat(char *a, char *b);
LinkedList *read_dataset3(char *filepath);

#endif
