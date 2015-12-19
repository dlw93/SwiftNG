//
// Created by david on 11/21/2015.
//

#ifndef GRAPHS_VERTEX_H
#define GRAPHS_VERTEX_H

#include "iterator.h"
#include "array.h"

typedef struct Vertex TVertex;

struct Vertex {
    int id;
    unsigned int indegree;
    unsigned int outdegree;
    unsigned int neighbours[];
};

void vertex_init(TVertex* v);

TIterator* vertex_get_successors(TVertex* v);

TIterator* vertex_get_predecessors(TVertex* v);

#endif //GRAPHS_VERTEX_H
