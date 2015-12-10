//
// Created by david on 11/21/2015.
//

#include "vertex.h"

TVertex* vertex_init() {
    return 0;
}

TIterator* vertex_get_successors(TVertex* v) {
    return iterator_init(((int*) v + 3), sizeof(int), v->outdegree);
}

TIterator* vertex_get_predecessors(TVertex* v) {
    return iterator_init(((int*) v + sizeof(v) - v->indegree), sizeof(int), v->indegree);
}

TArray* vertex_successors(TVertex* v) {
    TArray* arr = malloc(sizeof(TArray));

    arr->entry_count = v->outdegree;
    arr->entry_length = sizeof(int);
    arr->values = v->neighbours + 3;
    arr->values_init_addr = arr->values;

    return arr;
}

TArray* vertex_predecessors(TVertex* v) {
    TArray* arr = malloc(sizeof(TArray));

    arr->entry_count = v->indegree;
    arr->entry_length = sizeof(int);
    //arr->values = v->neighbours + 3 + v->;
    arr->values_init_addr = arr->values;

    return arr;
}
