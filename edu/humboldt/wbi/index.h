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

struct Index {
    TTreeSet* header_tree;
    THeaderEntry* header_entries;
    FILE* data_file;
};

struct HeaderEntry {
    int id;
    long pos;
    size_t size;
};

void index_init(TIndex* index, char* path);

void index_delete(TIndex* index);

void* index_get_entry(TIndex* index, int key);

#endif //GRAPHS_INDEX_H
