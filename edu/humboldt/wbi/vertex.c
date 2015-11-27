//
// Created by david on 11/21/2015.
//

#include "vertex.h"

TVertex* vertex_init() {
    return 0;
}

TIterator* vertex_get_successor(TVertex* v) {
    return iterator_init(((int*) v + 3), sizeof(int), v->outdegree);
}

TIterator* vertex_get_predecessor(TVertex* v) {
    return iterator_init(((int*) v + sizeof(v) - v->indegree), sizeof(int), v->indegree);
}
