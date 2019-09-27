#include <stdlib.h>
#include "image.h"
#include "separation.h"

Block *block_init()
{
    Block *block = malloc(sizeof(Block));

    return block;
}

void block_delete(Block *block)
{
    free(block);
}

Block *img_make_block(Img *image)
{
    Block *block = block_init();
    block->x = 0;
    block->y = 0;
    block->width = image->width;
    block->height = image->height;
    return block;
}

Img *img_from_block(Img *source, Block *block)
{
    Img *res = img_init(block->width, block->height);
    for (int y = 0; y < block->height; y++)
    {
        for (int x = 0; x < block->width; x++)
        {
            res->pixels[y * block->width + x] = source->pixels[(block->y + y) * source->width + block->x + x];
        }
    }
    return res;
}

void remove_white_margin(Img *image, Block *block)
{
    double threshold = 0.9999;

    // Remove left margin
    int x = block->x;
    // While the Xth column is white
    while (vertical_white_rate(image, block, x) > threshold && x < block->x + block->width)
        x++;
    // If the entire block is white, delete it
    if (x == block->x + block->width)
    {
        block_delete(block);
        return;
    }

    // Resize the block to exclude the left margin
    block->width = block->width - (x - block->x);
    block->x = x;

    // Remove right margin
    x = block->x + block->width - 1;
    // While the Xth column is white
    while (vertical_white_rate(image, block, x) > threshold && x > 0)
        x--;

    // Resize
    block->width = x - block->x;

    // Remove top margin
    int y = block->y;
    while (horizontal_white_rate(image, block, y) > threshold && y < block->y + block->height)
        y++;
    block->height = block->height - (y - block->y);
    block->y = y;

    // Remove bottom margin
    y = block->y + block->height - 1;
    while (horizontal_white_rate(image, block, y) > threshold && y > 0)
    {
        y--;
    }

    block->height = y - block->y;
}

double vertical_white_rate(Img *image, Block *block, int x)
{
    // Compute the average rate of white on the column
    double rate = 0.0;
    for (int y = 0; y < block->height; y++)
    {
        // Get the coordinate of the pixel in the entire image
        int pixelIndex = (block->y + y) * image->width + x;
        rate += image->pixels[pixelIndex];
    }
    // Average rate of white
    rate /= block->height;
    return rate;
}

double horizontal_white_rate(Img *image, Block *block, int y)
{
    // Compute the average rate of white on the column
    double rate = 0.0;
    for (int x = 0; x < block->width; x++)
    {
        int pixelIndex = y * image->width + (block->x + x);
        rate += image->pixels[pixelIndex];
    }

    // Average rate of white
    rate /= block->width;
    return rate;
}

Img resize(Img *image)
{
    int height = image->height;
    int width = image->width;
    int size = 28;
    double fx = width / size * 0.9999;
    double fy = height / height * 0.9999;
    double fix = 1 / fx;
    double fiy = 1 / fy;
    double sy1;
    double sy2;
    double sx1 = size * fx;
    double sx2 = sx1 + fx;
    double *res = malloc(sizeof(double) * size);

    //Check if the character is too big
    if (fx < 1 || fy < 1)
    {
        for (int y = 0; y < size; y++)
        {
            sy1 = size * fy;
            sy2 = sy1 + fy;
            //Calculate sy1, sy2, jstart, jend, devY1, devY2.
            for (int x = 0; x < size; x++)
            {
                /* code */
            }
        }
    }
    //Check if the character is too small
    else
    { //if(fx > 1 || fy>1)
    }
}
