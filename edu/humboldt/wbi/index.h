//
// Created by david on 12/5/2015.
//

#ifndef GRAPHS_INDEX_H
#define GRAPHS_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include "treeset.h"

typedef struct Index TIndex;
typedef struct HeaderEntry THeaderEntry;

typedef void (*tree_init_fn)(void *);

typedef void *(*tree_get_fn)(void *, int *);

typedef int (*tree_has_fn)(void *, int *);

typedef void (*tree_add_fn)(void *, void *, int *);

struct Index {
    void *header_tree;
    tree_init_fn init_fn;
    tree_get_fn get_fn;
	tree_add_fn add_fn;
	tree_has_fn has_fn;
    void *header_entries;
    size_t header_entry_size;
    FILE* data_file;
	long header_read_pos;
};

struct HeaderEntry {
    long pos;
    unsigned int size;
    int id[];
};

void index_init(TIndex* index, char* header_path, char* body_path);

void index_delete(TIndex* index);

void *index_get(TIndex *index, int *key);

int index_has(TIndex* index, int *key);

#endif //GRAPHS_INDEX_H
