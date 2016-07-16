#include <stdio.h>
#include "edu/humboldt/wbi/graph.h"
#include "edu/humboldt/wbi/matching.h"
#include "edu/humboldt/wbi/index.h"
#include "edu/humboldt/wbi/args.h"
#include "edu/humboldt/wbi/ngrams/ngramfns.h"

void compare(TIndex *idx) {
	FNGram fns[2] = { path_ngrams, neighbourhood_ngrams };
	int id1, id2, fn, n;
	
	while (scanf_s("%i %i %i %i", &fn, &n, &id1, &id2) != EOF) {
		TGraph *g1 = index_load_graph(idx, id1);
		TGraph *g2 = index_load_graph(idx, id2);
		TMatching *map = index_load_matching(idx, id1, id2, g1->node_count);
		double sim = graph_compare(g1, g2, map, fns[fn], n);

		printf("%i\t%i\t%f\n", id1, id2, sim);
	}
}

int main(int argc, char *argv[]) {
	TIndex idx;
	TArgs args;

	args_init(&args, argc, argv);

	if (args_has(&args, "index")) {
		char *index_path = args_get(&args, "index");

		if (index_init(&idx, index_path)) {
			fprintf(stderr, "Can't open index.\r\n");
			return 1;
		}
	}

	if (args_has(&args, "build-index")) {
		char *wf_path = args_get(&args, "workflows");
		char *map_path = args_get(&args, "mappings");

		index_build(&idx, wf_path, map_path);
	}
	else if (args_has(&args, "compare")) {
		compare(&idx);
	}
	else {
		fprintf(stderr, "You need to specify either --compare or --build-index mode.\r\n");
		return 1;
	}

	index_free(&idx);

	return 0;
}

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
}*/
