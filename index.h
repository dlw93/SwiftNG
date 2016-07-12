#ifndef INDEX_H
#define INDEX_H

#include "lib/sqlite/sqlite3.h"
#include "edu/humboldt/wbi/graph.h"
#include "edu/humboldt/wbi/matching.h"

void build_index(sqlite3 *db, char *wf_path, char *map_path);

TGraph *load_graph(sqlite3 *db, unsigned int id);

TMatching *load_matching(sqlite3 *db, unsigned int id1, unsigned int id2, size_t vertex_count);

#endif // INDEX