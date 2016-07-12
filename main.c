#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "lib/sqlite/sqlite3.h"
#include "edu/humboldt/wbi/graph.h"
#include "edu/humboldt/wbi/matching.h"
#include "edu/humboldt/wbi/hashmap.h"
#include "index.h"
#include "hashfns.h"
#include "args.h"

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
	size_t *item = alloca(sizeof(size_t) * k);
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

int compute_path_ngrams_node(TGraph *g, TArray *array, int array_index, int vertex_index, int n_counter, int n) {
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

			ngram_count += compute_path_ngrams_node(g, array, array_index + ngram_count, suc_index, n_counter - 1, n);
		}

		return ngram_count;
	}
	else {  // we completed an n-gram somewhere within the graph
		return 1;
		// take 2nd component and compute overlapping n-gram(s)
		//return compute_path_ngrams_node(g, array, array_index + 1, current_ngram.values[n - 2], n, n) + 1;
	}
}

TArray *compute_path_ngrams(TGraph *g, int n) {
	//unsigned int edge_count = graph_edge_count(g);
	unsigned int ngram_count = 0;
	TArray *ngrams = malloc(sizeof(TArray));

	array_init(ngrams, g->node_count * g->node_count * g->node_count / 4,
			   sizeof(int) * n); // max. possible amount of n-grams = (n^3 - n)/6

	for (int i = 0; i < g->node_count; ++i) {
		TVertex *v = graph_get_vertex(g, i);

		ngram_count += compute_path_ngrams_node(g, ngrams, ngram_count, i, n, n);
	}

	array_subarray(ngrams, 0, ngram_count);

	return ngrams;
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

TArray *compute_neighbourhood_ngrams(TGraph *g, int n) {
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

void compare(sqlite3 *db) {
	ngram_fn fns[2] = { &compute_path_ngrams, &compute_neighbourhood_ngrams };
	int id1, id2, fn, n;

	while (scanf_s("%i %i %i %i", &fn, &n, &id1, &id2) != EOF) {
		TGraph *g1 = load_graph(db, id1);
		TGraph *g2 = load_graph(db, id2);
		TMatching *map = load_matching(db, id1, id2, g1->node_count);
		double sim = graph_compare(g1, g2, map, fns[fn], n);

		printf("%i\t%i\t%f\n", id1, id2, sim);
	}
}

int main(int argc, char *argv[]) {
	sqlite3 *db;
	TArgs args;

	args_init(&args, argc, argv);

	char *index_path = args_get(&args, "index");

	if (sqlite3_open(index_path, &db)) {
		fprintf(stderr, "Can't connect to database: %s\r\n", sqlite3_errmsg(db));
		return 1;
	}

	if (args_has(&args, "build-index")) {
		char *wf_path = args_get(&args, "workflows");
		char *map_path = args_get(&args, "mappings");

		build_index(db, wf_path, map_path);
	}
	else if (args_has(&args, "compare")) {
		compare(db);
	}
	else {
		fprintf(stderr, "You need to specify either --compare or --build-index mode.\r\n");
		return 1;
	}

	sqlite3_close(db);

	return 0;
}

/*
THashMap map;
hashmap_init(&map, 1, &jenkins_oat_hash, &fnv_hash);
int a = 10, b = 11, c = 17;
hashmap_set(&map, "a", &a);
hashmap_set(&map, "izsrgfi", &b);
hashmap_set(&map, "ab", &c);
hashmap_set(&map, "a", &c);
printf("%d\r\n", *(int*) hashmap_get(&map, "a"));
printf("%d\r\n", *(int*) hashmap_get(&map, "ab"));
printf("%d\r\n", *(int*) hashmap_get(&map, "izsrgfi"));
// EXPECTED: 17, 17, 11
*/

/*double coverage(TGraph *graph, TArray *ngrams) {
	int *values = ngrams->values;
	int* visited = calloc(sizeof(int), graph->node_count);
	int nodes_covered = 0;

	for (int i = 0; i < ngrams->entry_count * ngrams->entry_length / sizeof(int); ++i) {
		if (!visited[values[i]]) {
			visited[values[i]] = 1;
			nodes_covered++;
		}
	}

	return (double) nodes_covered / (double) graph->node_count;
}

int main(int argc, char *argv[]) {
	sqlite3 *db;
	sqlite3_stmt *selectGraphStatement;

	if (sqlite3_open(argv[1], &db)) {
		fprintf(stderr, "Can't open workflow database: %s\n", sqlite3_errmsg(db));

		return 1;
	}

	sqlite3_prepare_v2(db, "SELECT id FROM Graph;", -1, &selectGraphStatement, NULL);

	while ((sqlite3_step(selectGraphStatement)) == SQLITE_ROW) {
		int graphId = sqlite3_column_int(selectGraphStatement, 0);

		TGraph *g = load_graph(db, graphId);

		printf("%d\t", graphId);

		for (int j = 2; j < 6; ++j) {
			TArray *ngrams = compute_path_ngrams(g, j);
			double cov = coverage(g, ngrams);

			printf("%f\t", cov);
		}

		for (int j = 4; j < 6; ++j) {
			TArray *ngrams = compute_neighbourhood_ngrams(g, j);
			double cov = coverage(g, ngrams);

			printf("%f\t", cov);
		}

		printf("\r\n");
	}

	sqlite3_finalize(selectGraphStatement);

	return 0;
}*/