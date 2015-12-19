//
// Created by david on 12/5/2015.
//

#include "index.h"

void index_init(TIndex* index, char* path) {
    FILE* f = fopen(path, "rb");

    index->header_file = f;
}

struct HeaderEntry index_get_header_entry(TIndex* index, int key) {
    THeaderEntry he;

    return he;
}

void* index_get_entry(TIndex* index, int key) {
    THeaderEntry he = index_get_header_entry(index, key);
    void* data = malloc(he.length);

    fread(data, sizeof(data), 1, index->data_file);

    return data;
}
