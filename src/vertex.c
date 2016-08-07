//
// Created by david on 11/21/2015.
//

#include <string.h>
#include "vertex.h"

void vertex_init(TVertex* v) {

}

TIterator* vertex_get_successors(TVertex* v) {
    return iterator_init(v->neighbours, sizeof(int), v->outdegree);
}

