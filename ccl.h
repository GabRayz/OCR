#ifndef CCL_H
#define CCL_H

#include "image.h"
#include "segmentation.h"

void merge_labels(Block *block, LinkedList *labels, int *hist);
void merge_two(Block *lineBlock, LinkedList *labels, Node *src, Node *dst, int *hist, int currentLabel);
LinkedList *ccl_segmentation(Img *source, bool whitespaces);
LinkedList *ccl_segment_block(Img *source, Block *block);
void hist_print(int *hist, int width, int height);
LinkedList *hist_to_images(Block *block, LinkedList *labels, int *hist);
void print_ccl(Img *source, LinkedList *labels);
LinkedList *create_hist(Img *source, Block *block, int *hist);
void propagate(Img *source, Block *block, Block *current, int *hist, int x, int y, int currentLabel);

#endif
