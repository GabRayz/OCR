#include "ccl.h"
#include "image.h"
#include "segmentation.h"
#include <stdlib.h>
#include <stdio.h>

LinkedList *ccl_segmentation(Img *source, bool whitespaces)
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
            // TODO: Add whitespaces between characters
            chars = list_concat(chars, ccl_segment_block(source, l->data));
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

LinkedList *ccl_segment_block(Img *source, Block *block)
{
    int *hist = calloc(block->width * block->height, sizeof(int));
    LinkedList *labels = create_hist(source, block, hist);
    return hist_to_images(block, labels, hist);
}

void hist_print(int *hist, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            printf("%d", hist[y * width + x]);
        }
        printf("\n");
    }
}

LinkedList *hist_to_images(Block *block, LinkedList *labels, int *hist)
{
    LinkedList *res = list_init();
    int currentLabel = 1;
    Node *n = labels->start;
    while (n)
    {
        Block *b = n->data;
        Img *image = img_init(b->width, b->height);
        for (int y = 0; y < b->height; y++)
        {
            for (int x = 0; x < b->width; x++)
            {
                if (hist[(y + b->y) * block->width + x + b->x] == currentLabel)
                {
                    image->pixels[y * b->width + x] = 0;
                }
            }
        }
        list_insert(res, node_init(image));
        n = n->next;
        currentLabel++;
    }
    return res;
}

void print_ccl(Img *source, LinkedList *labels)
{
    Node *n = labels->start;
    while (n)
    {
        Block *b = n->data;
        for (int y = b->y; y < b->y + b->height; y++)
        {
            for (int x = b->x; x < b->x + b->width; x++)
            {
                source->pixels[(y)*source->width + x] = 0;
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
    for (int x = 0; x < block->width; x++)
    {
        for (int y = 0; y < block->height; y++)
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
    if (x - current->x + 1 > current->width)
        current->width = x - current->x + 1;
    if (y - current->y + 1 > current->height)
        current->height = y - current->y + 1;
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
