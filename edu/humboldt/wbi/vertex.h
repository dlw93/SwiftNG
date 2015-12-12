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
    int indegree;
    int outdegree;
    int neighbours[];
};

TVertex* vertex_init();

TIterator* vertex_get_successors(TVertex* v);

TIterator* vertex_get_predecessors(TVertex* v);

TArray* vertex_successors(TVertex* v);

TArray* vertex_predecessors(TVertex* v);

void vertex_set_id(TVertex* v, int value);

#endif //GRAPHS_VERTEX_H
