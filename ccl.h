#ifndef CCL_H
#define CCL_H

#include "image.h"
#include "segmentation.h"

void ccl_segmentation(Img *source, Block *block);
void print_ccl(Img *source, Block *block, LinkedList *labels);
LinkedList *create_hist(Img *source, Block *block, int *hist);
void propagate(Img *source, Block *block, Block *current, int *hist, int x, int y, int currentLabel);

#endif
