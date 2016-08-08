#include "ngramfns.h"
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "defines.h"

int binomial(int n, int r) {
	if (r > n / 2) {
		r = n - r;
	}

	int ans = 1;

	for (int i = 1; i <= r; i++) {
		ans *= n - r + i;
		ans /= i;
	}

	return ans;
}

bool next_combination(size_t item[], size_t n, size_t N) {
	for (size_t i = 1; i <= n; ++i) {
		if (item[n - i] < N - i) {
			++item[n - i];

			for (size_t j = n - i + 1; j < n; ++j) {
				item[j] = item[j - 1] + 1;
			}

			return true;
		}
	}

	return false;
}

TArray* compute_combinations(size_t n, size_t k) {
	int bin = binomial(n, k);
	size_t *item = __alloca(size_t, k);
	TArray *combs_array = malloc(sizeof(TArray));

	array_init(combs_array, bin, k * sizeof(int));

	int *addr = combs_array->values;

	for (size_t i = 0; i < k; i++) {
		item[i] = i;
	}

	do {
		memcpy(addr, item, sizeof(int) * k);
		addr += k;
	}
	while (next_combination(item, k, n));

	return combs_array;
}

unsigned int compute_neighbourhood_ngram_count(TGraph *g, int n) {
	unsigned int ngram_count = 0;

	for (int i = 0; i < g->node_count; ++i) {
		TVertex *v = graph_get_vertex(g, i);

		if (v->indegree > 0 && v->outdegree >= n - 2) {
			ngram_count += binomial(v->outdegree, n - 2) * v->indegree;
		}
	}

	return ngram_count;
}

int compute_node_neighbourhood_ngrams(TGraph *g, TVertex *v, TNGram *ngrams, int n) {
	TArray* combs = compute_combinations(v->outdegree, n - 2);

	for (int i = 0; i < v->indegree; i++) {
		for (int j = 0; j < combs->entry_count; j++) {
			int *comb = array_get(combs, j);
			int *ngram = (int *) ngrams + (i * combs->entry_count + j) * n;
			int pred_index = v->neighbours[g->node_count - i - 1];

			*ngram = graph_get_vertex(g, pred_index)->id;
			*(ngram + 1) = v->id;

			for (int k = 2; k < n; k++) {
				int succ_index = v->neighbours[comb[k - 2]];
				*(ngram + k) = graph_get_vertex(g, succ_index)->id;
			}
		}
	}

	return v->indegree * combs->entry_count;
}

TArray *neighbourhood_ngrams(TGraph *g, int n) {
	unsigned int ngram_count = compute_neighbourhood_ngram_count(g, n);

	TArray *ngrams = malloc(sizeof(TArray));
	array_init(ngrams, ngram_count, sizeof(int) * n);

	int offset = 0;

	for (int i = 0; i < g->node_count; ++i) {
		TVertex *v = graph_get_vertex(g, i);

		if (v->indegree > 0 && v->outdegree >= n - 2) {
			offset += compute_node_neighbourhood_ngrams(g, v, array_get(ngrams, offset), n);
		}
	}

	return ngrams;
}

int path_ngrams_node(TGraph *g, TArray *array, int array_index, int vertex_index, int n_counter, int n) {
	TVertex *v = graph_get_vertex(g, vertex_index);

	((int *) array_get(array, array_index))[n_counter - 1] = v->id;

	if (v->outdegree == 0) { // we reached a sink
		return n_counter == 1;  // return, whether we completed an n-gram here or prune
	}
	else if (n_counter > 1) {   // we have not completed the current n-gram yet
		int ngram_count = 0;
		TIterator *it = vertex_get_successors(v);

		while (iterator_has_next(it)) {
			int suc_index = *(int *) iterator_next(it);

			for (int i = n_counter - 1; i < n; ++i) {
				((int *) array_get(array, array_index + ngram_count))[i] = ((int *) array_get(array, array_index))[i];
			}

			ngram_count += path_ngrams_node(g, array, array_index + ngram_count, suc_index, n_counter - 1, n);
		}

		return ngram_count;
	}
	else {  // we completed an n-gram somewhere within the graph
		return 1;
		// take 2nd component and compute overlapping n-gram(s)
		//return compute_path_ngrams_node(g, array, array_index + 1, current_ngram.values[n - 2], n, n) + 1;
	}
}

TArray *path_ngrams(TGraph *g, int n) {
	//unsigned int edge_count = graph_edge_count(g);
	unsigned int ngram_count = 0;
	TArray *ngrams = malloc(sizeof(TArray));

	array_init(ngrams, g->node_count * g->node_count * g->node_count / 4,
			   sizeof(int) * n); // max. possible amount of n-grams = (n^3 - n)/6

	for (int i = 0; i < g->node_count; ++i) {
		TVertex *v = graph_get_vertex(g, i);

		ngram_count += path_ngrams_node(g, ngrams, ngram_count, i, n, n);
	}

	array_subarray(ngrams, 0, ngram_count);

	return ngrams;
}
