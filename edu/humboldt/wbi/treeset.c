//
// Created by david on 12/18/2015.
//

#include <stdlib.h>
#include "treeset.h"

void treeset_init(TTreeSet *t) {
    t->root = 0;
}

void treeset_add(TTreeSet *t, void *entry, int *key) {
    TTreeNode** node = &t->root;
    int _key = *key;

    do {
        if (*node == 0) {
            *node = calloc(1, sizeof(TTreeNode));
        }

        node = &(*node)->children[_key % N];
        _key = _key / N;
    } while (_key > 0);

    if (*node == 0) {
        *node = calloc(1, sizeof(TTreeNode));
    }

    (*node)->value = entry;
}

TTreeNode *treeset_find(TTreeSet *t, int key) {
    TTreeNode* node = t->root;

    do {
        node = node->children[key % N];
        key = key / N;
    } while (key > 0);

    return node;
}

void *treeset_get(TTreeSet *t, int *key) {
    return treeset_find(t, *key)->value;
}
