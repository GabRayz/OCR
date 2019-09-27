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

double threshold = 0.9999;

void remove_white_margin(Img *img, Block *block)
{
    remove_top_margin(img, block);
    remove_bottom_margin(img, block);
    remove_left_margin(img, block);
    remove_right_margin(img, block);
}

Block *remove_top_margin(Img *img, Block *block)
{
    // Remove top margin
    int y = block->y;
    while (horizontal_white_rate(img, block, y) > threshold && y < block->y + block->height)
        y++;

    block->height = block->height - (y - block->y);
    block->y = y;

    return block;
}

Block *remove_bottom_margin(Img *img, Block *block)
{
    // Remove bottom margin
    int y = block->y + block->height - 1;
    while (horizontal_white_rate(img, block, y) > threshold && y > 0)
        y--;

    block->height = y - block->y;

    return block;
}

Block *remove_left_margin(Img *img, Block *block)
{
    // Remove left margin
    int x = block->x;
    // While the Xth column is white
    while (vertical_white_rate(img, block, x) > threshold && x < block->x + block->width)
        x++;

    block->width = block->width - (x - block->x);
    block->x = x;

    return block;
}

Block *remove_right_margin(Img *img, Block *block)
{
    // Remove right margin
    int x = block->x + block->width - 1;
    // While the Xth column is white
    while (vertical_white_rate(img, block, x) > threshold && x > 0)
        x--;

    block->width = x - block->x;

    return block;
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

void block_split_vertical(Img *image, Block *block, Block *res1, Block *res2)
{
    // Remove margins
    remove_white_margin(image, block);
    int x = block->x;
    // Go through columns until finding a white column
    while (vertical_white_rate(image, block, x) < threshold && x < block->x + block->width)
    {
        x++;
    }
    if (x == block->x + block->width)
    {
        block_delete(res1);
        block_delete(res2);
        return;
    }
    // Cut the image
    res1->x = block->x;
    res1->width = x - block->x;
    res2->x = x;
    res2->width = block->width - x;

    //
    res1->y = block->y;
    res1->height = block->height;
    res2->y = block->y;
    res2->height = block->height;
}

void block_split_horizontal(Img *image, Block *block, Block *top, Block *bottom)
{
    remove_white_margin(image, block);
    int y = block->y;

    int blackHeight = 0;
    int whiteHeight = 0;
    int current = 0;
    // If there are more white lines than previous black lines, split
    while (whiteHeight <= blackHeight && y < block->y + block->height)
    {
        double rate = horizontal_white_rate(image, block, y);
        // If the line is black
        if (rate < threshold)
        {
            blackHeight++;
            // If the last line was white
            if (current == 1)
            {
                whiteHeight = 0;
                blackHeight = 1;
            }
            current = 0;
        } // If the line is white
        else if (blackHeight > 5)
        {
            whiteHeight++;
            current = 1;
        }

        y++;
    }
    // Remove last white lines
    y -= whiteHeight;
    // Split
    top->y = block->y;
    top->height = y - block->y;
    bottom->y = y;
    bottom->height = block->height - y;

    //
    top->x = block->x;
    top->width = block->width;
    bottom->x = block->x;
    bottom->width = block->width;
}

// Img resize(Img *image)
// {
//     int height = image->height;
//     int width = image->width;
//     int size = 28;
//     double fx = width / size * 0.9999;
//     double fy = height / height * 0.9999;
//     double fix = 1 / fx;
//     double fiy = 1 / fy;
//     double sy1;
//     double sy2;
//     double sx1 = size * fx;
//     double sx2 = sx1 + fx;
//     double *res = malloc(sizeof(double) * size);

//     //Check if the character is too big
//     if (fx < 1 || fy < 1)
//     {
//         for (int y = 0; y < size; y++)
//         {
//             sy1 = size * fy;
//             sy2 = sy1 + fy;
//             //Calculate sy1, sy2, jstart, jend, devY1, devY2.
//             for (int x = 0; x < size; x++)
//             {
//                 /* code */
//             }
//         }
//     }
//     //Check if the character is too small
//     else
//     { //if(fx > 1 || fy>1)
//     }
// }
