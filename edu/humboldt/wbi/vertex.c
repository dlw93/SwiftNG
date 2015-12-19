//
// Created by david on 11/21/2015.
//

#include "vertex.h"

void vertex_init(TVertex* v) {

}

TIterator* vertex_get_successors(TVertex* v) {
    return iterator_init(((int*) v + 3), sizeof(int), v->outdegree);
}

TIterator* vertex_get_predecessors(TVertex* v) {
    return iterator_init(((int*) v + sizeof(v) - v->indegree), sizeof(int), v->indegree);
}
