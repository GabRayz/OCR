#include "ccl.h"
#include "image.h"
#include "segmentation.h"
#include <stdlib.h>

void ccl_segmentation(Img *source, Block *block)
{
    int *hist = calloc(block->width * block->height, sizeof(int));
    int currentLabel = 1;
    // Iterates through each pixel
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            // If the pixel is black and not labeled
            if (hist[y * block->width + x] == 0 && source->pixels[(y + block->y) * source->width + x + block->x] == 0)
            {
                // Start propagation and increment the label
                propagate(source, block, hist, x, y, currentLabel);
                currentLabel++;
            }
        }
    }
}

void propagate(Img *source, Block *block, int *hist, int x, int y, int currentLabel)
{
    if (x < 0 || x >= block->width || y < 0 || y >= block->height || hist[y * block->width + x] != 0)
        return;
    if (source->pixels[(y + block->y) * source->width + x + block->x] == 1)
        return;
    hist[y * block->width + x] = currentLabel;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (j == 0 && i == 0)
                continue;

            propagate(source, block, hist, x + j, y + i, currentLabel);
        }
    }
}
