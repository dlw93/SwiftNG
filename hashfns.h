#ifndef HASHFNS_H
#define HASHFNS_H

#include <stdlib.h>

// taken from https://en.wikipedia.org/wiki/Jenkins_hash_function
size_t jenkins_oat_hash(void * key);

// taken from http://www.eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
size_t fnv_hash(void * key);

#endif //HASHFNS_H
