//
// Created by david on 11/20/2015.
//

#ifndef GRAPHS_GRAPH_H
#define GRAPHS_GRAPH_H

#include "vertex.h"
#include "ngram.h"
#include "array.h"

typedef struct Graph TGraph;

typedef TArray* (* ngram_fn)(TGraph*, int);

struct Graph {
    unsigned int node_count;
    int* vertices;
};

/**
 * Initializes a graph with the specified amount of nodes.
 *
 * @param graph The graph handle to initialize.
 * @param node_count The amount of nodes the graph consists of.
 */
void graph_init(TGraph* graph, unsigned int node_count);

/**
 * Frees the memory occupied by a graph.
 *
 * @param graph The graph handle whose memory id to be freed.
 */
void graph_delete(TGraph* graph);

/**
 * Adds an edge between two specified nodes to the graph.
 *
 * @param A handle for the graph to add an edge to.
 * @param node1 The index of the source node.
 * @param node2 The index og the destination node.
 */
void graph_add_edge(TGraph* graph, int node1, int node2);

/**
 * Returns the amount of edges in the graph.
 *
 * @param graph A handle for the graph to retrieve the amount of edges from.
 */
unsigned int graph_edge_count(TGraph* graph);

/**
 * Returns a handle for the graph's vertex specified by its index.
 *
 * @param graph A handle for the graph to retrieve a vertex from.
 * @param index The vertices index.
 */
TVertex* graph_get_vertex(TGraph* graph, int index);

/**
 * Compares two graphs and return a value within [0, 1].
 */
double graph_compare(TGraph* g1, TGraph* g2, ngram_fn fn, int n);

#endif //GRAPHS_GRAPH_H
