//
// Created by david on 1/27/2016.
//

#include <stddef.h>
#include <stdlib.h>
#include "quadtree.h"

#define SIZE 4096   // bad!! make this dynamic somehow!

void quadtree_init(TQuadTree *t) {
    t->root = 0;
}

/**
 * @param x2 The x-component of the upper-left corner of the (sub-)square
 * @param y2 The y-component of the upper-left corner of the (sub-)square
 * @param size The width/height of the (sub-)square
 * @returns ret -> quadrant: 00 -> nw, 01 -> ne, 10 -> sw, 11 -> se
 */
static inline int get_direction(int x1, int y1, int x2, int y2, int size) {
    return (x1 >= x2 + (size >> 1)) | ((y1 >= y2 + (size >> 1)) << 1);
}

void quadtree_add(TQuadTree *t, void *entry, int *key) {
    int x = key[0];
    int y = key[1];
    int size = SIZE;
    TQuadTreeNode **node = &t->root;

    int xn = 0;
    int yn = 0;

    while (size > 1) {
        if (*node == 0) {
            *node = calloc(1, sizeof(TQuadTreeNode));
        }

        int direction = get_direction(x, y, xn, yn, size);

        node = &(*node)->children[direction];
        size = size >> 1;
        xn += size * (direction & 1);
        yn += size * (direction >> 1);
    }

    if (*node == 0) {
        *node = calloc(1, sizeof(TQuadTreeNode));
    }

    (*node)->value = entry;
}

void *quadtree_get(TQuadTree *t, int *key) {
    int x = key[0];
    int y = key[1];
    int size = SIZE;
    TQuadTreeNode *node = t->root;

    int xn = 0;
    int yn = 0;

    while (size > 1) {
        int direction = get_direction(x, y, xn, yn, size);

        node = node->children[direction];
        size = size >> 1;
        xn += size * (direction & 1);
        yn += size * (direction >> 1);
    }

    return node->value;
}
