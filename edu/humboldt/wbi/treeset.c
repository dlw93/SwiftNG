//
// Created by david on 12/18/2015.
//

#include <stdlib.h>
#include "treeset.h"

void treeset_init(TTreeSet* t, unsigned int n) {
    t->n = n;
    t->root = 0;
}

void treeset_delete(TTreeSet* t) {

}

void treeset_add(TTreeSet* t, int key, void* entry) {
    TTreeNode** node = &t->root;

    do {
        if (*node == 0) {
            *node = calloc(1, sizeof(TTreeNode));
        }

        node = &(*node)->children[key % t->n];
        key = key / t->n;
    } while (key > 0);

    if (*node == 0) {
        *node = calloc(1, sizeof(TTreeNode));
    }

    (*node)->value = entry;
}

void* treeset_get(TTreeSet* t, int key) {
    TTreeNode* node = t->root;

    do {
        node = node->children[key % t->n];
        key = key / t->n;
    } while (key > 0);

    return node->value;
}

int treeset_contains(TTreeSet* t, int key) {

}
