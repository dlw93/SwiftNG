//
// Created by david on 11/20/2015.
//

#include <stdlib.h>
#include "graph.h"

void graph_init(TGraph* graph, unsigned int node_count) {
    graph->node_count = node_count;
    graph->id_map = calloc((node_count + 4) * node_count, sizeof(int));
    graph->vertices = graph->id_map + node_count;
}

void graph_delete(TGraph* graph) {
    free(graph->id_map);
    free(graph);
}

void graph_add_edge(TGraph* graph, int node1, int node2) {
    TVertex* v1 = graph_get_vertex(graph, node1);
    TVertex* v2 = graph_get_vertex(graph, node2);

    v1->neighbours[v1->outdegree++] = node2;
    v2->neighbours[graph->node_count - ++v2->indegree] = node1;
}

TVertex* graph_get_vertex(TGraph* graph, int index) {
    int offset = index * (graph->node_count + 3);
    int* addr = graph->vertices + offset;

    return (TVertex*) addr;
}

int graph_edge_count(TGraph* graph) {
    int edge_count = 0;

    for (int i = 0; i < graph->node_count; ++i) {
        edge_count += graph_get_vertex(graph, i)->indegree;
    }

    return edge_count;
}

double graph_compare(TGraph* g1, TGraph* g2, ngram_fn fn, int n) {
    size_t intersection_size = 0;
    size_t entry_count = 0;

    TArray* a1 = fn(g1, n);
    TArray* a2 = fn(g2, n);

    TArray* a_min = a1->entry_count < a2->entry_count ? a1 : a2;
    TArray* a_max = a1 == a_min ? a2 : a1;

    array_sort(a_min, ngram_compare);
    array_sort(a_max, ngram_compare);

    entry_count = a_min->entry_count + a_max->entry_count;
    int find_index;

    for (int i = 0; i < a_min->entry_count; ++i) {
        // get n-gram from one graph...
        TNGram* ngram = array_get(a_min, i);

        // ...and look for it in the other graph
        find_index = array_find(a_max, ngram, ngram_compare);

        // if we found an n-gram in both graph's sets (i.e. an element in both sets intersection)...
        if (find_index >= 0) {
            // ...we forget everything before the position we found the n-gram at (since the arrays are sorted)
            array_subarray(a_max, find_index, a_max->entry_count);

            intersection_size++;
        }
    }

    array_delete(a1);
    array_delete(a2);

    return 1.0 - ((double) intersection_size / (double) (entry_count - intersection_size));
}
