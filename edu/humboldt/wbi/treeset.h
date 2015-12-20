//
// Created by david on 12/18/2015.
//

#ifndef GRAPHS_TREESET_H
#define GRAPHS_TREESET_H

#define N 8

typedef struct TreeSet TTreeSet;
typedef struct TreeNode TTreeNode;

struct TreeNode {
    void* value;
    TTreeNode* children[N];
};

struct TreeSet {
    unsigned int n;
    TTreeNode* root;
};

void treeset_init(TTreeSet* t, unsigned int n);

void treeset_delete(TTreeSet* t);

void treeset_add(TTreeSet* t, int key, void* entry);

void* treeset_get(TTreeSet* t, int key);

int treeset_contains(TTreeSet* t, int key);

#endif //GRAPHS_TREESET_H
