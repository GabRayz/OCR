#include "ccl.h"
#include "image.h"
#include "segmentation.h"
#include <stdlib.h>
#include <stdio.h>

void ccl_segmentation(Img *source, Block *block)
{
    int *hist = calloc(block->width * block->height, sizeof(int));
    LinkedList *labels = create_hist(source, block, hist);
    print_ccl(source, block, labels);
}

void print_ccl(Img *source, Block *block, LinkedList *labels)
{
    Node *n = labels->start;
    while (n)
    {
        Block *b = n->data;
        // printf("%d\n", b->width);
        for (int y = b->y; y < b->y + b->height; y++)
        {
            for (int x = b->x; x < b->x + b->width; x++)
            {
                source->pixels[(y) * source->width + x] = 0;
            }
        }
        n = n->next;
    }

    img_save(source, "res/ccl.png");
}

LinkedList *create_hist(Img *source, Block *block, int *hist)
{
    LinkedList *labels = list_init();
    int currentLabel = 1;
    // Iterates through each pixel
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            // If the pixel is black and not labeled
            if (hist[y * block->width + x] == 0 && source->pixels[(y + block->y) * source->width + x + block->x] == 0)
            {
                // Create the block that will store the size and location
                Block *current = block_init();
                current->x = x;
                current->y = y;
                // Start propagation and increment the label
                propagate(source, block, current, hist, x, y, currentLabel);
                currentLabel++;
                Node *node = node_init(current);
                list_insert(labels, node);
            }
        }
    }

    return labels;
}

void propagate(Img *source, Block *block, Block *current, int *hist, int x, int y, int currentLabel)
{
    // Check if the pixel exists and is not labeled
    if (x < 0 || x >= block->width || y < 0 || y >= block->height || hist[y * block->width + x] != 0)
        return;
    if (source->pixels[(y + block->y) * source->width + x + block->x] == 1)
        return;
    hist[y * block->width + x] = currentLabel;
    // Update block's coordinates
    if (x < current->x)
        current->x = x;
    if (y < current->y)
        current->y = y;
    if (x - current->x > current->width)
        current->width = x - current->x + 2;
    if (y - current->y > current->height)
        current->height = y - current->y + 2;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (j == 0 && i == 0)
                continue;

            propagate(source, block, current, hist, x + j, y + i, currentLabel);
        }
    }
}
