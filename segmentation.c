#include <stdlib.h>
#include <stdio.h>
#include "image.h"
#include "linkedlist.h"
#include "segmentation.h"
#include <assert.h>
#include <stdbool.h>
#include <stdbool.h>

LinkedList *segmentation(Img *source, bool whitespaces)
{
    Block *block = img_make_block(source);
    LinkedList *paragraphs = block_split_vertical(source, block, true);
    LinkedList *chars = list_init();

    Node *p = paragraphs->start;
    while (p)
    {
        LinkedList *lines = line_split(source, p->data);
        Node *l = lines->start;
        while (l)
        {
            chars = list_concat(chars, character_split(source, l->data, whitespaces));
            if (whitespaces)
            {
                Block *spacing = block_init();
                spacing->label = ' ';

                list_insert(chars, node_init(spacing));
            }

            l = l->next;
        }

        if (whitespaces && p->next)
        {
            Block *newline = block_init();
            newline->label = '\n';

            list_insert(chars, node_init(newline));
            list_insert(chars, node_init(newline));
        }

        // Maybe free lines with content & blocks ?
        p = p->next;
    }
    // Maybe free paragraphs with content & blocks ?
    return chars;
}

Block *block_init()
{
    Block *block = malloc(sizeof(Block));
    block->x = 0;
    block->y = 0;
    block->width = 0;
    block->height = 0;
    block->label = '\0';

    return block;
}

void block_delete(Block *block)
{
    assert(block != NULL);
    free(block);
}

Block *img_make_block(Img *image)
{
    Block *block = block_init();
    block->label = image->label;
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

    block->height = y - block->y + 1;

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

    block->width = x - block->x + 1;

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

LinkedList *block_split_vertical(Img *image, Block *block, bool shouldRetry)
{
    /* Split the block in two, put them in a linked list */

    // Remove margins
    remove_white_margin(image, block);
    int x = block->x;
    int blackWidth = 0;
    int whiteWidth = 0;
    int currentColor = 0;
    // Go through columns until finding a white column
    while (whiteWidth <= 20 && x < block->x + block->width)
    {
        double rate = vertical_white_rate(image, block, x);
        // If line is black
        if (rate < threshold)
        {
            blackWidth++;
            // If the last line was white
            if (currentColor == 1)
            {
                whiteWidth = 0;
                blackWidth = 1;
            }
            currentColor = 0;
        }
        else if (blackWidth > 5)
        {
            whiteWidth++;
            currentColor = 1;
        }
        x++;
    }

    // If no cut is needed
    if (x == block->x + block->width)
    {
        if (shouldRetry)
            return block_split_horizontal(image, block, false);
        else
        {
            LinkedList *res = list_init();
            // Put the main block in the list
            Node *node = node_init(block);
            res->start = node;
            res->end = node;
            return res;
        }
    }
    // If the block has been cut
    Block *res1 = block_init();
    Block *res2 = block_init();

    // Cut the image
    res1->x = block->x;
    res1->width = x - block->x;
    res2->x = x;
    res2->width = (block->x + block->width) - x;

    // Set y attributes
    res1->y = block->y;
    res1->height = block->height;
    res2->y = block->y;
    res2->height = block->height;
    LinkedList *child1 = block_split_horizontal(image, res1, false);
    LinkedList *child2 = block_split_vertical(image, res2, true);
    return list_concat(child1, child2);
}

LinkedList *block_split_horizontal(Img *image, Block *block, bool shouldRetry)
{
    /* Split the block in two, put child blocks in a linked list */

    // Remove margins
    remove_white_margin(image, block);
    int y = block->y;

    int blackHeight = 0;
    int whiteHeight = 0;
    int currentColor = 0;
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

    // If no cut is needed
    if (y == block->y + block->height)
    {
        if (shouldRetry)
            return block_split_vertical(image, block, false);
        else
        {
            LinkedList *res = list_init();
            // Put the main block in the list
            Node *node = node_init(block);
            res->start = node;
            res->end = node;
            return res;
        }
    }
    // If the block has been cut
    Block *res1 = block_init();
    Block *res2 = block_init();

    // Cut the image
    res1->y = block->y;
    res1->height = y - block->y;
    res2->y = y;
    res2->height = block->height - y;

    // Set y attributes
    res1->x = block->x;
    res1->width = block->width;
    res2->x = block->x;
    res2->width = block->width;

    LinkedList *child1 = block_split_horizontal(image, res1, false);
    LinkedList *child2 = block_split_vertical(image, res2, true);

    return list_concat(child1, child2);
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

LinkedList *character_split(Img *image, Block *block, bool whitespaces)
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
            remove_white_margin(image, current);
            list_insert(res, node_init(current));
            current = NULL;
        }
        // If the column is black and the previous column is white (no block defined)
        else if (rate < threshold && current == NULL)
        {
            Block *last = res->end != NULL ? res->end->data : NULL;
            // Insert whitespace if needed
            if (whitespaces && last != NULL && x > last->x + last->width + block->height / 3)
            {
                Block *space = block_init();

                space->x = last->x + last->width;
                space->width = x - space->x;
                space->y = block->y;
                space->height = block->height;
                space->label = ' ';

                list_insert(res, node_init(space));
            }

            // Create a new block
            current = block_init();
            current->x = x - 1;
            current->y = block->y;
            current->height = block->height;
        }
    }
    return res;
}
