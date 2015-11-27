//
// Created by david on 11/21/2015.
//

#include <stdlib.h>
#include "iterator.h"

TIterator* iterator_init(void* addr, size_t step, int length) {
    TIterator* it = malloc(sizeof(TIterator));

    it->step = step;
    it->addr = addr - step;
    it->limit = addr + (step * (length - 1));

    return it;
}

int iterator_has_next(TIterator* iterator) {
    return iterator->addr != iterator->limit;
}

void* iterator_next(TIterator* iterator) {
    iterator->addr += iterator->step;

    return iterator->addr;
}
