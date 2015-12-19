//
// Created by david on 12/18/2015.
//

#ifndef GRAPHS_TREESET_H
#define GRAPHS_TREESET_H

typedef struct TreeSet TTreeSet;
typedef struct TreeNode TTreeNode;

struct TreeNode {
    void* value;
    TTreeNode* children[];
};

struct TreeSet {
    int n;
    TTreeNode* children[];
};

void treeset_init(TTreeSet* t, int n);

void treeset_add(TTreeSet* t, int key, void* entry);

void* treeset_get(TTreeSet* t, int key);

int treeset_contains(TTreeSet* t, int key);

#endif //GRAPHS_TREESET_H
