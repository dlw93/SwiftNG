//
// Created by david on 12/5/2015.
//

#ifndef GRAPHS_INDEX_H
#define GRAPHS_INDEX_H

typedef struct Index TIndex;

struct Index {

};

void index_load_from_file(TIndex* index, char* path);

void* index_get_entry(TIndex* index, int key);

#endif //GRAPHS_INDEX_H
