//
// Created by david on 12/18/2015.
//

#ifndef GRAPHS_TREESET_H
#define GRAPHS_TREESET_H

#define N 2

typedef struct TreeSet TTreeSet;
typedef struct TreeNode TTreeNode;

struct TreeNode {
    void* value;
    TTreeNode* children[N];
};

struct TreeSet {
    TTreeNode* root;
};

void treeset_init(TTreeSet *t);

void treeset_add(TTreeSet *t, void *entry, int key);

void *treeset_get(TTreeSet *t, int key);

int treeset_has(TTreeSet *t, int key);

#endif //GRAPHS_TREESET_H
