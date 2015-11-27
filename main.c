#include <stdio.h>
#include "edu/humboldt/wbi/graph.h"

int cmp(TGraph* g, TArray* array, int array_index, int vertex_index, int n, int n_max) {
    if (array_index >= array->entry_count) {
        return 0;
    }

    TNGram current_ngram = {
            array_get(array, array_index)
    };

    current_ngram.values[n - 1] = vertex_index;

    if (n > 1) {
        TVertex* v = graph_get_vertex(g, vertex_index);
        TIterator* it = vertex_get_successor(v);
        int ngram_count = 0;

        if (v->outdegree == 0) { // vertex is a sink, can't complete n-gram
            return 0;
        }

        while (iterator_has_next(it)) {
            int suc_index = *(int*) iterator_next(it);

            ngram_count += cmp(g, array, array_index + ngram_count, suc_index, n - 1, n_max);
            ngram_count += cmp(g, array, array_index + ngram_count, suc_index, n_max, n_max);
        }

        return ngram_count;
    }
    else {
        return 1;
    }
}

TArray* fake_cmp(TGraph* g, int n) {
    int edge_count = graph_edge_count(g);
    unsigned int ngram_count = 0;
    TArray* ngrams = malloc(sizeof(TArray));

    array_init(ngrams, edge_count, sizeof(int) * n);

    for (int i = 0; i < g->node_count; ++i) {
        TVertex* v = graph_get_vertex(g, i);

        if (v->indegree == 0) {
            ngram_count += cmp(g, ngrams, ngram_count, i, n, n);
        }
    }

    array_subarray(ngrams, 0, ngram_count);

    return ngrams;
}

int main(char* argv[], int argc) {
    TGraph g1, g2;
    int map[] = {0, 1, 2, -1, 3, 4};

    graph_init(&g1, 6);
    graph_init(&g2, 5);

    graph_add_edge(&g1, 0, 1);
    graph_add_edge(&g1, 0, 2);
    graph_add_edge(&g1, 0, 3);
    graph_add_edge(&g1, 1, 4);
    graph_add_edge(&g1, 2, 4);
    graph_add_edge(&g1, 3, 5);
    graph_add_edge(&g1, 4, 5);

    graph_add_edge(&g2, 0, 1);
    graph_add_edge(&g2, 0, 2);
    graph_add_edge(&g2, 1, 3);
    graph_add_edge(&g2, 2, 3);
    graph_add_edge(&g2, 3, 4);

    printf("%f", graph_compare(&g1, &g2, &fake_cmp, 3));

    return 0;
}

/*if(edge_count == 7) {   // g1
    array_set(ngrams, int, 0, 0);
    array_set(ngrams, int, 1, 1);
    array_set(ngrams, int, 2, 3);
    array_set(ngrams, int, 3, 0);
    array_set(ngrams, int, 4, 2);
    array_set(ngrams, int, 5, 3);
    array_set(ngrams, int, 6, 1);
    array_set(ngrams, int, 7, 3);
    array_set(ngrams, int, 8, 4);
    array_set(ngrams, int, 9, 2);
    array_set(ngrams, int, 10, 3);
    array_set(ngrams, int, 11, 4);
    array_set(ngrams, int, 12, 0);
    array_set(ngrams, int, 13, -1);
    array_set(ngrams, int, 14, 4);

    ngrams->entry_count = 5;
}
else {  // g2
    array_set(ngrams, int, 0, 0);
    array_set(ngrams, int, 1, 1);
    array_set(ngrams, int, 2, 3);
    array_set(ngrams, int, 3, 0);
    array_set(ngrams, int, 4, 2);
    array_set(ngrams, int, 5, 3);
    array_set(ngrams, int, 6, 1);
    array_set(ngrams, int, 7, 3);
    array_set(ngrams, int, 8, 4);
    array_set(ngrams, int, 9, 2);
    array_set(ngrams, int, 10, 3);
    array_set(ngrams, int, 11, 4);

    ngrams->entry_count = 4;
}*/