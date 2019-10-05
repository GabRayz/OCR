#include <stdlib.h>
#include <stdio.h>
#include "image.h"
#include "separation.h"

Block *block_init()
{
    Block *block = malloc(sizeof(Block));
    block->x = 0;
    block->y = 0;
    block->width = 0;
    block->height = 0;

    return block;
}

void block_delete(Block *block)
{
    free(block);
}

Node *node_init(Block *block)
{
    Node *node = malloc(sizeof(Node));
    node->block = block;
    node->next = NULL;

    return node;
}

void node_free(Node *list)
{
    free(list);
}

LinkedList *list_init()
{
    LinkedList *list = malloc(sizeof(LinkedList));
    list->start = NULL;
    list->end = NULL;

    return list;
}

void list_free(LinkedList *list)
{
    free(list);
}

void list_insert(LinkedList *list, Node *node)
{
    if (list->start == NULL)
    {
        list->start = node;
        list->end = node;
    }
    list->end->next = node;
    list->end = node;
}

int list_length(LinkedList *list)
{
    int res = 0;
    Node *node = list->start;
    while (node != NULL)
    {
        node = node->next;
        res++;
    }
    return res;
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

double vertical_char_white_rate(Img *image, Block *block, int x)
{
    // Compute the average rate of white on the column
    double rate = 0.0;
    for (int y = 0; y < block->height; y++)
    {
        // Get the coordinate of the pixel in the entire image
        int pixelIndex = (block->y + y) * image->width + x;
        // Check if pixel at x & x - 1 is black
        rate += image->pixels[pixelIndex] || (x != block->x && image->pixels[pixelIndex - 1]);
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
    int currentColor = 0;
    // If there are more white lines than previous black lines, split
    while (whiteHeight <= blackHeight * 1.5 && y < block->y + block->height)
    {
        double rate = horizontal_white_rate(image, block, y);
        // If the line is black
        if (rate < threshold)
        {
            blackHeight++;
            // If the last line was white
            if (currentColor == 1)
            {
                whiteHeight = 0;
                blackHeight = 1;
            }
            currentColor = 0;
        } // If the line is white
        else if (blackHeight > 5)
        {
            whiteHeight++;
            currentColor = 1;
        }

        y++;
    }
    // Remove last white lines
    y -= whiteHeight;
    // Split
    top->y = block->y;
    top->height = y - block->y;
    bottom->y = y;
    bottom->height = block->height - top->height;

    top->x = block->x;
    top->width = block->width;
    bottom->x = block->x;
    bottom->width = block->width;
}

LinkedList *line_split(Img *image, Block *block)
{
    LinkedList *res = list_init();

    Block *current = NULL;

    for (int y = block->y; y < block->y + block->height; y++)
    {
        double rate = horizontal_white_rate(image, block, y);

        // If the line is white and previous line is black
        if (current != NULL && (rate > threshold || y == block->y + block->height - 1))
        {
            current->height = y - current->y;
            list_insert(res, node_init(current));
            current = NULL;
        }
        // If the line is black and the previous line is white (no block defined)
        else if (rate < threshold && current == NULL)
        {
            // Create a new block
            current = block_init();
            current->y = y;
            current->x = block->x;
            current->width = block->width;
        }
    }

    return res;
}

LinkedList *character_split(Img *image, Block *block)
{
    LinkedList *res = list_init();

    Block *current = NULL;

    for (int x = block->x; x < block->x + block->width; x++)
    {
        double rate = vertical_char_white_rate(image, block, x);

        // If the column is white and previous column is black
        if (current != NULL && (rate > threshold || x == block->x + block->width - 1))
        {
            current->width = x - current->x;
            list_insert(res, node_init(current));
            current = NULL;
        }
        // If the column is black and the previous column is white (no block defined)
        else if (rate < threshold && current == NULL)
        {
            // Create a new block
            current = block_init();
            current->x = x;
            current->y = block->y;
            current->height = block->height;
        }
    }

    return res;
}
