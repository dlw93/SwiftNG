#ifndef INDEX_H
#define INDEX_H

#include "../lib/sqlite/sqlite3.h"
#include "graph.h"
#include "matching.h"

typedef sqlite3* TIndex;

int index_init(TIndex *index, char *path);

void index_destroy(TIndex *index);

void index_build(TIndex *index, char *wf_path, char *map_path);

TGraph *index_load_graph(TIndex *index, unsigned int id);

TMatching *index_load_matching(TIndex *index, unsigned int id1, unsigned int id2, size_t vertex_count);

#endif // INDEX