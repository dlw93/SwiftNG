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
    FILE* header_file;
    TTreeSet* header_tree;
    FILE* data_file;
};

struct HeaderEntry {
    long pos;
    size_t length;
};

void index_init(TIndex* index, char* path);

void* index_get_entry(TIndex* index, int key);

#endif //GRAPHS_INDEX_H
