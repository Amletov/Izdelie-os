#ifndef BITMAP_H
#define BITMAP_H

#include <stdlib.h>
#include <math.h>
#include "shared.h"

#define MAP_OUT_OF_RANGE -1
#define MAP_NULL -2

typedef struct bitmap
{
    u8 *maps;     //
    i32 capacity; // Максимальное количесво эл. в bitmap
} bitmap_t;

bitmap_t *create_bitmap(i32 capacity)
{
    bitmap_t *bitmap = (bitmap_t *)malloc(sizeof(bitmap_t));
    bitmap->capacity = capacity;
    bitmap->maps = (u8 *)calloc(ceil(capacity / sizeof(u8)), sizeof(u8));

    return bitmap;
}

int lock_bitmap_element(bitmap_t *bitmap, int index)
{
    if (bitmap->maps == NULL)
    {
        return MAP_NULL;
    }
    if (index < 0 && index >= bitmap->capacity)
    {
        return MAP_OUT_OF_RANGE;
    }

    int map_index = index / bitmap->capacity;
    i8 in_map_index = index % (sizeof(u8) * 8);
    bitmap->maps[map_index] |= 1 << in_map_index;

    return 0;
}

int unlock_bitmap_element(bitmap_t *bitmap, int index)
{
    if (bitmap->maps == NULL)
    {
        return MAP_NULL;
    }
    if (index < 0 && index >= bitmap->capacity)
    {
        return MAP_OUT_OF_RANGE;
    }

    int map_index = index / bitmap->capacity;
    i8 in_map_index = index % (sizeof(u8) * 8);
    bitmap->maps[map_index] &= ~(1 << in_map_index);

    return 0;
}

int is_bitmap_element_locked(bitmap_t *bitmap, int index)
{
    if (bitmap->maps == NULL)
    {
        return MAP_NULL;
    }
    if (index < 0 && index >= bitmap->capacity)
    {
        return MAP_OUT_OF_RANGE;
    }

    int map_index = index / bitmap->capacity;
    i8 in_map_index = index % (sizeof(u8) * 8);

    return (bitmap->maps[map_index] & 1 << in_map_index) != 0;
}

#endif