#include <stdio.h>
#include <sys/time.h>
#include "edu/humboldt/wbi/graph.h"

int compute_path_ngrams_node(TGraph* g, TArray* array, int array_index, int vertex_index, int n_counter, int n) {
    TVertex* v = graph_get_vertex(g, vertex_index);

    ((int*) array_get(array, array_index))[n_counter - 1] = v->id;

    if (v->outdegree == 0) { // we reached a sink
        return n_counter == 1;  // return, whether we completed an n-gram here or prune
    }
    else if (n_counter > 1) {   // we have not completed the current n-gram yet
        int ngram_count = 0;
        TIterator* it = vertex_get_successors(v);

        while (iterator_has_next(it)) {
            int suc_index = *(int*) iterator_next(it);

            ngram_count += compute_path_ngrams_node(g, array, array_index + ngram_count, suc_index, n_counter - 1, n);
        }

        return ngram_count;
    }
    /*else {  // we completed an n-gram somewhere within the graph
        // take 2nd component and compute overlapping n-gram(s)
        return compute_path_ngrams_node(g, array, array_index + 1, current_ngram.values[n - 2], n, n) + 1;
    }*/
}

TArray* compute_path_ngrams(TGraph* g, int n) {
    int edge_count = graph_edge_count(g);
    unsigned int ngram_count = 0;
    TArray* ngrams = malloc(sizeof(TArray));

    array_init(ngrams, edge_count, sizeof(int) * n);

    for (int i = 0; i < g->node_count; ++i) {
        TVertex* v = graph_get_vertex(g, i);

        if (v->outdegree > 0) {
            ngram_count += compute_path_ngrams_node(g, ngrams, ngram_count, i, n, n);
        }
    }

    array_subarray(ngrams, 0, ngram_count);

    return ngrams;
}

int compute_neighbourhood_ngram_count(TGraph* g, int n) {
    int ngram_count = 0;

    for (int i = 0; i < g->node_count; ++i) {
        TVertex* v = graph_get_vertex(g, i);

        if (v->indegree > 0 && v->outdegree > 0) {
            ngram_count += v->indegree * (v->outdegree - n + 3);
        }
    }

    return ngram_count;
}

int compute_node_neighbourhood_ngrams(TGraph* g, TVertex* v, TNGram* ngrams, int n) {
    int pred_ngrams = v->outdegree - n + 3; // how many n-grams per predecessor

    for (int j = 1; j <= v->indegree; ++j) {
        for (int i = 0; i < pred_ngrams; ++i) {
            int* ngram = (int*) ngrams + i * n;

            int pred_index = v->neighbours[g->node_count - j];

            *ngram = graph_get_vertex(g, pred_index)->id;  // predecessor id
            *(ngram + 1) = v->id; // vertex id

            for (int k = 2; k < n; ++k) {
                int succ_index = v->neighbours[i + k];

                *(ngram + k) = graph_get_vertex(g, succ_index)->id; // successor id
            }
        }
    }

    return v->indegree * pred_ngrams;
}

TArray* compute_neighbourhood_ngrams(TGraph* g, int n) {
    int ngram_count = compute_neighbourhood_ngram_count(g, n);

    TArray* ngrams = malloc(sizeof(TArray));
    array_init(ngrams, ngram_count, sizeof(int) * n);

    int offset = 0;

    for (int i = 0; i < g->node_count; ++i) {
        TVertex* v = graph_get_vertex(g, i);

        if (v->indegree > 0 && v->outdegree >= n - 2) {
            offset += compute_node_neighbourhood_ngrams(g, v, array_get(ngrams, offset), n);
        }
    }

    return ngrams;
}

int main(char* argv[], int argc) {
    TGraph g1, g2;
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

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    printf("%f\n", graph_compare(&g1, &g2, &compute_path_ngrams, 3));

    gettimeofday(&stop, NULL);
    printf("took %lu\n", stop.tv_usec - start.tv_usec); // microseconds

    /*char* index_path = argv[1];
    int g1_id = atoi(argv[2]);
    int g2_id = atoi(argv[3]);
    TIndex index;

    index_load_from_file(&index, argv[1]);
    TGraph* g1 = index_get_entry(&index, g1_id);
    TGraph* g2 = index_get_entry(&index, g2_id);

    printf("%f", graph_compare(g1, g2, &compute_path_ngrams, 3));*/

    return 0;
}
