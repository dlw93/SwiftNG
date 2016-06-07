#include <stdio.h>
#include <string.h>
#include "lib/sqlite/sqlite3.h"
#include "edu/humboldt/wbi/graph.h"
#include "edu/humboldt/wbi/matching.h"

int binomial(int n, int r) {
    if(r > n / 2) {
        r = n - r;
    }
    
    int ans = 1;

    for(int i = 1; i <= r; i++) {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

void binomial_enum_rec(int n, int k, int p, int low, int* c, int* data, int* combs) {
    int high = n - k + p;

    for (int i = low; i <= high; i++) {
        data[p] = i;

        if (p >= k - 1) {
            int* comb = combs + (*c)++ * k;

            for (int j = 0; j < k; j++) {
                comb[j] = data[j];
            }
        }
        else {
            binomial_enum_rec(n, k, p + 1, i + 1, c, data, combs);
        }
    }
}

TArray* binomial_enum(int n, int k) {
    int bin = binomial(n, k);
    int* combs = malloc(sizeof(int) * bin * k);
    int data[k];
    int count = 0;

    binomial_enum_rec(n, k, 0, 0, &count, data, combs);
    
    TArray *combs_array = malloc(sizeof(TArray));
    combs_array->entry_count = bin;
    combs_array->entry_length  = k * sizeof(int);
    combs_array->values = combs;
    combs_array->values_init_addr = combs_array->values;
    
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
    TArray* combs = binomial_enum(v->outdegree, n - 2);
    
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

TGraph *load_graph(sqlite3 *db, unsigned int id) {
    TGraph *graph = NULL;
    sqlite3_stmt *selectGraphStatement, *selectVertexStatement, *selectEdgeStatement;

    sqlite3_prepare_v2(db, "SELECT nodeCount FROM Graph WHERE id=?;", -1, &selectGraphStatement, NULL);
    sqlite3_prepare_v2(db, "SELECT number, name FROM Vertex WHERE graphId=?;", -1, &selectVertexStatement, NULL);
    sqlite3_prepare_v2(db, "SELECT vertex1, vertex2 FROM Edge WHERE graphId=?;", -1, &selectEdgeStatement, NULL);

    sqlite3_bind_int(selectGraphStatement, 1, id);
    sqlite3_bind_int(selectVertexStatement, 1, id);
    sqlite3_bind_int(selectEdgeStatement, 1, id);

    if ((sqlite3_step(selectGraphStatement)) == SQLITE_ROW) {
        int node_count = sqlite3_column_int(selectGraphStatement, 0);
        graph = graph_init(node_count);
    }

    while ((sqlite3_step(selectVertexStatement)) == SQLITE_ROW) {
        int vertex_number = sqlite3_column_int(selectVertexStatement, 0);
        //const char *vertex_name = sqlite3_column_text(selectVertexStatement, 1);
        TVertex *v = graph_get_vertex(graph, vertex_number);
        //v->title = malloc(strlen(vertex_name) + 1);
        v->id = vertex_number;

        //strcpy(v->title, vertex_name);
    }

    while ((sqlite3_step(selectEdgeStatement)) == SQLITE_ROW) {
        int vertex1 = sqlite3_column_int(selectEdgeStatement, 0);
        int vertex2 = sqlite3_column_int(selectEdgeStatement, 1);

        graph_add_edge(graph, vertex1, vertex2);
    }

    sqlite3_finalize(selectGraphStatement);
    sqlite3_finalize(selectVertexStatement);
    sqlite3_finalize(selectEdgeStatement);

    return graph;
}

TMatching *load_matching(sqlite3 *db, unsigned int id1, unsigned int id2, size_t vertex_count) {
    TMatching *matching = malloc(sizeof(TMatching) * vertex_count);
    sqlite3_stmt *selectMatchingStatement;

    sqlite3_prepare_v2(db,
                       "SELECT vertexNumber1, vertexNumber2, similarity FROM Matching WHERE graphPairId=(SELECT id FROM GraphPair WHERE graphId1=? AND graphId2=?);",
                       -1, &selectMatchingStatement, NULL);
    sqlite3_bind_int(selectMatchingStatement, 1, id1);
    sqlite3_bind_int(selectMatchingStatement, 2, id2);

    memset(matching, -1, sizeof(TMatching) * vertex_count);

    while ((sqlite3_step(selectMatchingStatement)) == SQLITE_ROW) {
        int vertex_number_1 = sqlite3_column_int(selectMatchingStatement, 0);
        int vertex_number_2 = sqlite3_column_int(selectMatchingStatement, 1);
        double similarity = sqlite3_column_double(selectMatchingStatement, 2);

        matching[vertex_number_1] = (TMatching){ vertex_number_2, similarity };
    }

    sqlite3_finalize(selectMatchingStatement);

    return matching;
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

int main(int argc, char *argv[]) {
	sqlite3 *db;
	ngram_fn fns[2] = { &compute_path_ngrams, &compute_neighbourhood_ngrams };
	
	if (sqlite3_open(argv[1], &db)) {
		fprintf(stderr, "Can't open workflow database: %s\n", sqlite3_errmsg(db));

		return 1;
	}
	
	int id1, id2, fn, n;

    while (scanf("%i %i %i %i", &fn, &n, &id1, &id2) != EOF) {
        TGraph *g1 = load_graph(db, id1);
        TGraph *g2 = load_graph(db, id2);
        TMatching *map = load_matching(db, id1, id2, g1->node_count);
        double sim = graph_compare(g1, g2, map, fns[fn], n);

        printf("%i\t%i\t%f\n", id1, id2, sim);
    }
	
	sqlite3_close(db);
	
	return 0;
}
