//
// Created by david on 11/22/2015.
//

#include <stdlib.h>
#include "ngram.h"

TNGram* ngram_init(int n) {
    TNGram* ngram = malloc(sizeof(int) * n);

    return ngram;
}

void ngram_set(TNGram* ngram, int index, int value) {
    ngram->values[index] = value;
}

int ngram_get(TNGram* ngram, int index) {
    return ngram->values[index];
}

/**
 * Compares to n-grams and returns "0" in case of equality, "-1" in case of "n1 < n2" and "1" in case of "n1 > n2".
 */
int ngram_compare(void* ngram1, void* ngram2, size_t ngram_length) {
    size_t n = ngram_length / sizeof(int);
    int* n1 = (int*) ngram1;
    int* n2 = (int*) ngram2;

    for (int i = 0; i < n; ++i) {
        if (n1[i] != n2[i]) {
            return (n1[i] > n2[i]) - (n1[i] < n2[i]);
        }
    }

    return 0;
}
