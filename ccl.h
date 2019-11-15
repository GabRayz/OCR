#ifndef CCL_H
#define CCL_H

#include "image.h"
#include "segmentation.h"

void ccl_segmentation(Img *source, Block *block);
void propagate(Img *source, Block *block, int *hist, int x, int y, int currentLabel);

#endif
