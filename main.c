#include <stdio.h>
#include "edu/humboldt/wbi/graph.h"
#include "edu/humboldt/wbi/index.h"

int get_ngram(TGraph* g, TArray* array, int array_index, int vertex_index, int n_counter, int n) {
    TVertex* v = graph_get_vertex(g, vertex_index);
    TNGram current_ngram = {
            array_get(array, array_index)
    };

    current_ngram.values[n_counter - 1] = v->id;

    if (v->outdegree == 0) { // we reached a sink
        return n_counter == 1;  // return, whether we completed an n-gram here or prune
    }
    else if (n_counter > 1) {   // we have not completed the current n-gram yet
        int ngram_count = 0;
        TIterator* it = vertex_get_successors(v);

        while (iterator_has_next(it)) {
            int suc_index = *(int*) iterator_next(it);

            ngram_count += get_ngram(g, array, array_index + ngram_count, suc_index, n_counter - 1, n);
        }

        return ngram_count;
    }
    else {  // we completed an n-gram somewhere within the graph
        // take 2nd component and compute overlapping n-gram(s)
        return get_ngram(g, array, array_index + 1, current_ngram.values[n - 2], n, n) + 1;
    }
}

TArray* compute_path_ngrams(TGraph* g, int n) {
    int edge_count = graph_edge_count(g);
    unsigned int ngram_count = 0;
    TArray* ngrams = malloc(sizeof(TArray));

    array_init(ngrams, edge_count, sizeof(int) * n);

    for (int i = 0; i < g->node_count; ++i) {
        TVertex* v = graph_get_vertex(g, i);

        if (v->indegree == 0) {
            ngram_count += get_ngram(g, ngrams, ngram_count, i, n, n);
        }
    }

    array_subarray(ngrams, 0, ngram_count);

    return ngrams;
}

TArray* compute_node_neighbourhood_ngrams(TGraph* g, TVertex* v, int n) {
    int pred_ngrams = v->outdegree - n + 3; // how many n-grams per predecessor
    int ngram_count = v->indegree * pred_ngrams;
    TArray* ngrams = malloc(sizeof(TArray));

    array_init(ngrams, ngram_count, sizeof(int) * n);

    for (int j = 1; j <= v->indegree; ++j) {
        for (int i = 0; i < pred_ngrams; ++i) {
            int* ngram = (int*) ngrams->values + i * n;

            *ngram = v->neighbours[g->node_count - j];  // predecessor index
            *(ngram + 1) = -1; // vertex index

            for (int k = 2; k < n; ++k) {
                *(ngram + k) = v->neighbours[i + k]; // successor index
            }
        }
    }

    return ngrams;
}

TArray* compute_neighbourhood_ngrams(TGraph* g, int n) {
    int edge_count = graph_edge_count(g);
    unsigned int ngram_count = 0;
    TArray* ngrams = malloc(sizeof(TArray));

    array_init(ngrams, edge_count, sizeof(int) * n);

    for (int i = 0; i < g->node_count; ++i) {
        TVertex* v = graph_get_vertex(g, i);

        if (v->indegree > 0 && v->outdegree > 0) {

        }
    }

    return ngrams;
}

int main(char* argv[], int argc) {
    /*TGraph g1, g2;
    int map[] = {0, 1, 2, -1, 3, 4};

    graph_init(&g1, 6);
    graph_init(&g2, 5);

    graph_get_vertex(&g1, 0)->id = map[0];
    graph_get_vertex(&g1, 1)->id = map[1];
    graph_get_vertex(&g1, 2)->id = map[2];
    graph_get_vertex(&g1, 3)->id = map[3];
    graph_get_vertex(&g1, 4)->id = map[4];
    graph_get_vertex(&g1, 5)->id = map[5];

    graph_get_vertex(&g2, 0)->id = 0;
    graph_get_vertex(&g2, 1)->id = 1;
    graph_get_vertex(&g2, 2)->id = 2;
    graph_get_vertex(&g2, 3)->id = 3;
    graph_get_vertex(&g2, 4)->id = 4;

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

    printf("%f", graph_compare(&g1, &g2, &compute_path_ngrams, 3));*/

    char* index_path = argv[1];
    int g1_id = atoi(argv[2]);
    int g2_id = atoi(argv[3]);
    TIndex index;

    index_load_from_file(&index, argv[1]);
    TGraph* g1 = index_get_entry(&index, g1_id);
    TGraph* g2 = index_get_entry(&index, g2_id);

    printf("%f", graph_compare(g1, g2, &compute_path_ngrams, 3));

    return 0;
}
