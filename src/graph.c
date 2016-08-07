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

	return graph;
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

	return (TVertex*)addr;
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

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) < (b) ? (b) : (a))

/*double levenshtein(const char *s1, const char *s2) {
	size_t s1len, s2len, maxlen;
	unsigned int x, y, lastdiag, olddiag;
	s1len = strlen(s1);
	s2len = strlen(s2);
	maxlen = MAX2(s1len, s2len);
	unsigned int *column = alloca(sizeof(int) * (s1len + 1));

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
}*/

/*void map(TGraph *g1, TGraph *g2, double* map[]) {
	for (int i = 0; i < g1->node_count; i++) {
		for (int j = 0; j < g2->node_count; j++) {
			const char *title1 = graph_get_vertex(g1, i)->title;
			const char *title2 = graph_get_vertex(g2, j)->title;

			map[i][j] = levenshtein(title1, title2);
		}
	}
}*/



double coverage(TGraph *graph, TArray *ngrams) {
	int *values = ngrams->values;
	int *visited = calloc(sizeof(int), graph->node_count);
	int nodes_covered = 0;

	for (int i = 0; i < ngrams->entry_count * ngrams->entry_length / sizeof(int); ++i) {
		if (!visited[values[i]]) {
			visited[values[i]] = 1;
			nodes_covered++;
		}
	}

	free(visited);

	return (double)nodes_covered / (double)graph->node_count;
}

void map_ids(TArray *array, TMatching *map) {
	int *values = array->values;

	for (int i = 0; i < array->entry_count * array->entry_length / sizeof(int); ++i) {
		values[i] = map[values[i]].value;
	}
}

double* reverse_map(TMatching *matching, size_t matching_length, size_t length) {
	double* rmap = malloc(sizeof(double) * length);

	for (int i = 0; i < matching_length; i++) {
		if (matching[i].value >= 0) {
			rmap[matching[i].value] = matching[i].score;
		}
	}

	return rmap;
}

double graph_compare(TGraph *g1, TGraph *g2, TMatching* map, FNGram fn, unsigned int n) {
	double* rmap = reverse_map(map, g1->node_count, MAX2(g1->node_count, g2->node_count));
	double sim = 0.0;
	TArray *a1 = fn(g1, n);
	TArray *a2 = fn(g2, n);

	map_ids(a1, map);
	array_sort(a1, ngram_compare);
	array_sort(a2, ngram_compare);

	if (MIN2(a1->entry_count, a2->entry_count) == 0) {
		sim = -1.0;
	}
	else {
		TArray *intersection = array_intersect(a1, a2, ngram_compare);

		for (int i = 0; i < intersection->entry_count; i++) {
			int *ngram = *(int**)array_get(intersection, i);
			double ngram_sim = 0.0;

			for (int j = 0; j < n; j++) {
				ngram_sim += rmap[ngram[j]];
			}

			sim += ngram_sim / (double)n;
		}

		sim = sim / ((double)(g1->node_count + g2->node_count) - sim);
	}

	//array_delete(a1);
	//array_delete(a2);

	return sim;
}
