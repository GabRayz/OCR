#include "block.h"

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
    assert(block != NULL);
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
