//
// Created by david on 11/21/2015.
//

#ifndef GRAPHS_ITERATOR_H
#define GRAPHS_ITERATOR_H

#include <stdlib.h>

typedef struct Iterator TIterator;

struct Iterator {
    void* addr;
    void* limit;
    size_t step;
};

TIterator* iterator_init(void* addr, size_t step, int length);
int iterator_has_next(TIterator* iterator);
void* iterator_next(TIterator* iterator);

#endif //GRAPHS_ITERATOR_H
