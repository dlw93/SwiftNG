//
// Created by david on 11/20/2015.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"

TGraph *graph_init(unsigned int node_count) {
    TGraph *graph = calloc((node_count + 3) * node_count + 1, sizeof(int));
    graph->node_count = node_count;
}

void graph_delete(TGraph* graph) {
    free(graph);
}

void graph_add_edge(TGraph *graph, unsigned int node1, unsigned int node2) {
    TVertex* v1 = graph_get_vertex(graph, node1);
    TVertex* v2 = graph_get_vertex(graph, node2);

    v1->neighbours[v1->outdegree++] = node2;
    v2->neighbours[graph->node_count - ++v2->indegree] = node1;
}

TVertex* graph_get_vertex(TGraph* graph, int index) {
    int offset = index * (graph->node_count + 3);
    unsigned int *addr = &graph->vertices[offset];

    return (TVertex*) addr;
}

TVertex* graph_get_vertex_by_id(TGraph* graph, int id) {

}

unsigned int graph_edge_count(TGraph* graph) {
    unsigned int edge_count = 0;

    for (int i = 0; i < graph->node_count; ++i) {
        edge_count += graph_get_vertex(graph, i)->indegree;
    }

    return edge_count;
}

unsigned int graph_vertex_count(TGraph *graph) {
    return graph->node_count;
}

/*double compute_ngram_coverage(TGraph* graph, TArray* ngrams) {
    
}

TArray* compute_ngrams_min_coverage(TGraph* graph, ngram_fn fn, double coverage) {
    int n = 5;
    
    TArray* a = fn(graph, n);
    double c = compute_ngram_coverage(graph, a);
    
    while(c < coverage) {
        array_delete(a);
        a = fn(graph, --n);
        c = compute_ngram_coverage(graph, a);
    }
}*/

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX2(a, b) ((a) < (b) ? (b) : (a))

double levenshtein(const char *s1, const char *s2) {
    size_t s1len, s2len, maxlen;
    unsigned int x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    maxlen = MAX2(s1len, s2len);
    unsigned int column[s1len + 1];

    for (y = 1; y <= s1len; y++) {
        column[y] = y;
    }

    for (x = 1; x <= s2len; x++) {
        column[0] = x;

        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }

    return (double) (maxlen - column[s1len]) / (double) maxlen;
}

/*void map(TGraph *g1, TGraph *g2, double* map[]) {
    for (int i = 0; i < g1->node_count; i++) {
        for (int j = 0; j < g2->node_count; j++) {
            const char *title1 = graph_get_vertex(g1, i)->title;
            const char *title2 = graph_get_vertex(g2, j)->title;

            map[i][j] = levenshtein(title1, title2);
        }
    }
}*/

double graph_compare(TGraph *g1, TGraph *g2, ngram_fn fn, unsigned int n) {
    if (n == 0) {
        return -1;
    }

    TArray* a1 = fn(g1, n);
    TArray* a2 = fn(g2, n);

    TArray* a_min = a1->entry_count < a2->entry_count ? a1 : a2;
    TArray* a_max = a1 == a_min ? a2 : a1;

    double sim = -1;

    if (a_min->entry_count > 0) {
        array_sort(a_min, ngram_compare);
        array_sort(a_max, ngram_compare);

        sim = array_compare(a_min, a_max, ngram_compare);

        array_delete(a1);
        array_delete(a2);
    }

    return sim;
}

/*double graph_compare(TGraph *g1, TGraph *g2, ngram_fn fn, unsigned int n) {
    if (n == 0) {
        return -1;
    }

    TArray *a1 = fn(g1, n);
    TArray *a2 = fn(g2, n);

    if (a1->entry_count == 0 || a2->entry_count == 0) {
        return graph_compare(g1, g2, fn, n - 1);
    }

    double map_array[g1->node_count][g2->node_count];

    for (int i = 0; i < g1->node_count; i++) {
        for (int j = 0; j < g2->node_count; j++) {
            const char *title1 = graph_get_vertex(g1, i)->title;
            const char *title2 = graph_get_vertex(g2, j)->title;

            map_array[i][j] = levenshtein(title1, title2);
        }
    }

    double graph_sim = 0.0;
    int mapping[a1->entry_count];

    for (int i = 0; i < a1->entry_count; ++i) {
        int *ngram1 = array_get(a1, i);
        double max = 0.0;

        for (int j = 0; j < a2->entry_count; ++j) {
            int *ngram2 = array_get(a2, j);
            double ngram_sim = 0.0;

            for (int k = 0; k < n; ++k) {
                ngram_sim += map_array[ngram1[k]][ngram2[k]];
            }

            if (ngram_sim > max) {
                max = ngram_sim;
                mapping[i] = j;
            }
        }

        graph_sim += max / (double) n;
    }

    return graph_sim / MAX2(a1->entry_count, a2->entry_count);
}*/
