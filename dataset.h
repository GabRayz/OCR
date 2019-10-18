#ifndef DATASET_H
#define DATASET_H

#include "image.h"

void dataset_to_pixels(Img **images, int dataCount);
void create_dataset_from_img(char* source, char *destination);
char *concat(char *a, char *b);
Img **images_from_list(Img *source, LinkedList *chars, int *count);
LinkedList *read_dataset(char *filepath);

#endif
