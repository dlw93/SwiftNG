//
// Created by david on 1/27/2016.
//

#ifndef GRAPHS_QUADTREE_H
#define GRAPHS_QUADTREE_H

typedef struct QuadTree TQuadTree;
typedef struct QuadTreeNode TQuadTreeNode;

struct QuadTreeNode {
    void *value;
    TQuadTreeNode *children[4];
};

struct QuadTree {
    TQuadTreeNode *root;
};

void quadtree_init(TQuadTree *t);

void quadtree_add(TQuadTree *t, void *entry, int *key);

void *quadtree_get(TQuadTree *t, int *key);

int quadtree_has(TQuadTree *t, int *key);

#endif //GRAPHS_QUADTREE_H
