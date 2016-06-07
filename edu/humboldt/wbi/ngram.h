//
// Created by david on 11/22/2015.
//

#ifndef GRAPHS_NGRAM_H
#define GRAPHS_NGRAM_H

typedef struct NGram TNGram;

struct NGram {
    int* values;
};

TNGram* ngram_init(int n);
int ngram_compare(void* ngram1, void* ngram2, size_t ngram_length);

#endif //GRAPHS_NGRAM_H
