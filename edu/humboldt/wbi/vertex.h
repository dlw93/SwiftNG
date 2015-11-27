//
// Created by david on 11/21/2015.
//

#ifndef GRAPHS_VERTEX_H
#define GRAPHS_VERTEX_H

#include "iterator.h"

typedef struct Vertex TVertex;

struct Vertex {
    int id;
    int indegree;
    int outdegree;
    int neighbours[];
};

TVertex* vertex_init();
TIterator* vertex_get_successor(TVertex* v);
TIterator* vertex_get_predecessor(TVertex* v);

#endif //GRAPHS_VERTEX_H
