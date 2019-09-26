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

void remove_white_margin(Img *image, Block *block)
{
    // Remove left margin
    int x = block->x;
    // While the Xth column is white
    while (vertical_white_rate(image, block, x) > 0.95)
    {
        x++;
    }

    // If the entire block is white, delete it
    if (x == block->width)
    {
        block_delete(block);
    }
    else
    {
        // Resize the block to exclude the left margin
        block->width -= x - block->x;
        block->x = x;
    }

    // Remove right margin
    x = block->x + block->width;
    // While the Xth column is white
    while (vertical_white_rate(image, block, x) > 0.95)
    {
        x--;
    }
    // Resize
    block->width -= block->x + block->width - x;

    // Remove top margin
    int y = block->y;
    while (horizontal_white_rate(image, block, y) > 0.95)
    {
        y++;
    }
    block->height -= y - block->y;
    block->y = y;

    // Remove bottom margin
    y = block->y + block->height;
    while (horizontal_white_rate(image, block, y) > 0.95)
    {
        y--;
    }
    block->height -= block->y + block->height - y;
}

double vertical_white_rate(Img *image, Block *block, int x)
{
    // Compute the average rate of white on the column
    double rate = 0.0;
    for (int y = 0; y < block->height; y++)
    {
        // Get the coordinate of the pixel in the entire image
        int pixelIndex = (block->y + y) * image->width + (block->x + x);
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
        int pixelIndex = (block->y + y) * image->width + (block->x + x);
        rate += image->pixels[pixelIndex];
    }

    // Average rate of white
    rate /= block->height;
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
