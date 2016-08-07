#include "index.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include "../../../lib/cJSON/cJSON.h"
#include "hashmap.h"
#include "hashfns.h"

typedef void(*FJsonProcessor)(char*, va_list);

typedef struct {
	sqlite3_stmt *insert_vertex_statement;
	sqlite3_stmt *insert_edge_statement;
	sqlite3_stmt *insert_graph_statement;
	sqlite3_stmt *insert_graph_pair_statement;
	sqlite3_stmt *insert_matching_statement;
} TPreparedStatements;

int extract_id_from_uri(char* resource_uri) {
	char *res_uri_wo_prefix = resource_uri + 39;	// length of prefix "<http://www.myexperiment.org/workflows/" is 39
	char *slash = strchr(res_uri_wo_prefix, '/');
	size_t span = slash - res_uri_wo_prefix;
	char *id_str = alloca(span);
	memcpy(id_str, res_uri_wo_prefix, span);

	return atoi(id_str);
}

int* ids_from_filename(char *filename) {
	char *underscore = strchr(filename, '_');
	char *dot = strchr(filename, '.');

	*underscore = 0;
	*dot = 0;

	int *ids = malloc(sizeof(int) * 2);

	ids[0] = atoi(filename);
	ids[1] = atoi(underscore + 1);

	return ids;
}

char* read_file_to_string(FILE* file) {
	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	rewind(file);

	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, file);

	string[fsize] = 0;

	return string;
}

void setup_db(sqlite3 *db) {
	char err_msg_graph[] = "Cannot create table \"Graph\"";
	char err_msg_vertex[] = "Cannot create table \"Vertex\"";
	char err_msg_edge[] = "Cannot create table \"Edge\"";
	char err_msg_graph_pair[] = "Cannot create table \"GraphPair\"";
	char err_msg_matching[] = "Cannot create table \"Matching\"";

	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Graph (id INTEGER, nodeCount INTEGER,PRIMARY KEY (id));", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Vertex (graphId INTEGER, number INTEGER,name TEXT,PRIMARY KEY (graphId, number),FOREIGN KEY (graphId) REFERENCES Graph(id));", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Edge (graphId INTEGER, vertex1 INTEGER,vertex2 INTEGER,PRIMARY KEY (graphId, vertex1, vertex2),FOREIGN KEY (graphId) REFERENCES Graph(id));", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS GraphPair (id INTEGER PRIMARY KEY AUTOINCREMENT,graphId1 INTEGER,graphId2 INTEGER,matchingCount INTEGER,FOREIGN KEY (graphId1) REFERENCES Graph(id),FOREIGN KEY (graphId2) REFERENCES Graph(id));", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Matching (graphPairId INTEGER, vertexNumber1 INTEGER,vertexNumber2 INTEGER,similarity REAL,PRIMARY KEY (graphPairId, vertexNumber1, vertexNumber2),FOREIGN KEY (graphPairId) REFERENCES GraphPair(id));", NULL, NULL, NULL);

	sqlite3_exec(db, "PRAGMA JOURNAL_MODE = MEMORY", NULL, NULL, NULL);
	sqlite3_exec(db, "PRAGMA SYNCHRONOUS = OFF", NULL, NULL, NULL);
}

void setup_statements(sqlite3 *db, TPreparedStatements *stmts) {
	sqlite3_prepare_v2(db, "INSERT INTO Vertex (graphId, number, name) VALUES (?, ?, ?);", -1, &stmts->insert_vertex_statement, NULL);
	sqlite3_prepare_v2(db, "INSERT INTO Edge (graphId, vertex1, vertex2) VALUES (?, ?, ?);", -1, &stmts->insert_edge_statement, NULL);
	sqlite3_prepare_v2(db, "INSERT INTO Graph (id, nodeCount) VALUES (?, ?);", -1, &stmts->insert_graph_statement, NULL);
	sqlite3_prepare_v2(db, "INSERT INTO GraphPair (graphId1, graphId2, matchingCount) VALUES (?, ?, ?);", -1, &stmts->insert_graph_pair_statement, NULL);
	sqlite3_prepare_v2(db, "INSERT INTO Matching (graphPairId, vertexNumber1, vertexNumber2, similarity) VALUES (?, ?, ?, ?);", -1, &stmts->insert_matching_statement, NULL);
}

void finialize_statements(sqlite3 *db, TPreparedStatements *stmts) {
	sqlite3_finalize(stmts->insert_graph_statement);
	sqlite3_finalize(stmts->insert_edge_statement);
	sqlite3_finalize(stmts->insert_vertex_statement);
	sqlite3_finalize(stmts->insert_graph_pair_statement);
	sqlite3_finalize(stmts->insert_matching_statement);
}

void read_workflow(char *filepath, va_list args) {
	sqlite3 *db = va_arg(args, sqlite3*);
	THashMap *id_map = va_arg(args, THashMap*);
	TPreparedStatements *stmts = va_arg(args, TPreparedStatements*);

	FILE *f;
	fopen_s(&f, filepath, "rb");
	char *json_str = read_file_to_string(f);
	fclose(f);

	cJSON *root = cJSON_Parse(json_str);
	cJSON *nodes = cJSON_GetObjectItem(root, "nodes");
	cJSON *edges = cJSON_GetObjectItem(root, "edges");

	char* resource_uri = cJSON_GetObjectItem(root, "resource_uri")->valuestring;
	int id = extract_id_from_uri(resource_uri);

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

	cJSON *node = nodes->child;
	size_t node_count = 0;

	while (node) {
		char *node_id = cJSON_GetObjectItem(node, "id")->valuestring;
		char *node_title = cJSON_GetObjectItem(node, "dcterms:title")->valuestring;

		sqlite3_bind_int(stmts->insert_vertex_statement, 1, id);
		sqlite3_bind_int(stmts->insert_vertex_statement, 2, node_count);
		sqlite3_bind_text(stmts->insert_vertex_statement, 3, node_title, strlen(node_title), SQLITE_STATIC);

		if (sqlite3_step(stmts->insert_vertex_statement) != SQLITE_DONE) {
			fprintf(stderr, "Failed to insert vertex \"%s\"\r\n", node_id);
		}

		sqlite3_reset(stmts->insert_vertex_statement);

		hashmap_set(id_map, node_id, node_count++);	// inserting "int" instead of "void*" --> relies on sizeof(int) <= sizeof(void*)

		node = node->next;
	}

	cJSON *edge = edges->child;

	while (edge) {
		char *edge_start_id = edge->child->valuestring;
		char *edge_end_id = edge->child->next->valuestring;

		int start = hashmap_get(id_map, edge_start_id);
		int end = hashmap_get(id_map, edge_end_id);

		sqlite3_bind_int(stmts->insert_edge_statement, 1, id);
		sqlite3_bind_int(stmts->insert_edge_statement, 2, start);
		sqlite3_bind_int(stmts->insert_edge_statement, 3, end);

		if (sqlite3_step(stmts->insert_edge_statement) != SQLITE_DONE) {
			fprintf(stderr, "Failed to insert edge from \"%s\" to \"%s\"\r\n", edge_start_id, edge_end_id);
		}

		sqlite3_reset(stmts->insert_edge_statement);

		edge = edge->next;
	}

	sqlite3_bind_int(stmts->insert_graph_statement, 1, id);
	sqlite3_bind_int(stmts->insert_graph_statement, 2, node_count);

	if (sqlite3_step(stmts->insert_graph_statement) != SQLITE_DONE) {
		fprintf(stderr, "Failed to insert graph %d: %s\r\n", id, sqlite3_errmsg(db));
	}

	sqlite3_reset(stmts->insert_graph_statement);

	sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);

	free(json_str);
	cJSON_Delete(root);
}

void read_mapping(char *filepath, va_list args) {
	sqlite3 *db = va_arg(args, sqlite3*);
	THashMap *id_map = va_arg(args, THashMap*);
	TPreparedStatements *stmts = va_arg(args, TPreparedStatements*);

	FILE *f;
	fopen_s(&f, filepath, "rb");
	char *json_str = read_file_to_string(f);
	fclose(f);

	cJSON *root = cJSON_Parse(json_str);
	cJSON *mappings = cJSON_GetObjectItem(root, "processor_mapping");

	int *ids = ids_from_filename(strrchr(filepath, '\\') + 1);
	int length = cJSON_GetArraySize(mappings);

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

	sqlite3_bind_int(stmts->insert_graph_pair_statement, 1, ids[0]);
	sqlite3_bind_int(stmts->insert_graph_pair_statement, 2, ids[1]);
	sqlite3_bind_int(stmts->insert_graph_pair_statement, 3, length);

	if (sqlite3_step(stmts->insert_graph_pair_statement) != SQLITE_DONE) {
		fprintf(stderr, "Failed to insert graph pair (%d, %d)\r\n", ids[0], ids[1]);
	}

	int graph_pair_id = sqlite3_last_insert_rowid(db);

	sqlite3_reset(stmts->insert_graph_pair_statement);

	cJSON *mapping = mappings->child;

	while (mapping) {
		char* id1_str = cJSON_GetObjectItem(mapping, "processor1")->valuestring;
		char* id2_str = cJSON_GetObjectItem(mapping, "processor2")->valuestring;
		double score = cJSON_GetObjectItem(mapping, "score")->valuedouble;
		int id1 = hashmap_get(id_map, id1_str);
		int id2 = hashmap_get(id_map, id2_str);

		sqlite3_bind_int(stmts->insert_matching_statement, 1, graph_pair_id);
		sqlite3_bind_int(stmts->insert_matching_statement, 2, id1);
		sqlite3_bind_int(stmts->insert_matching_statement, 3, id2);
		sqlite3_bind_double(stmts->insert_matching_statement, 4, score);

		if (sqlite3_step(stmts->insert_matching_statement) != SQLITE_DONE) {
			fprintf(stderr, "Failed to insert matching between \"%s\" and \"%s\": %s\r\n", id1_str, id2_str, sqlite3_errmsg(db));
		}

		sqlite3_reset(stmts->insert_matching_statement);

		mapping = mapping->next;
	}

	sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);

	free(json_str);
	cJSON_Delete(root);
}

#ifdef _WIN32
void apply_to_dir(char *dir_path, FJsonProcessor fn, ...) {
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char sPath[MAX_PATH];

	sprintf_s(sPath, MAX_PATH, "%s\\*.*", dir_path);

	va_list args, args_cp;
	va_start(args, fn);

	if ((hFind = FindFirstFile(sPath, &fdFile)) != INVALID_HANDLE_VALUE) {
		do {
			sprintf_s(sPath, MAX_PATH, "%s\\%s", dir_path, fdFile.cFileName);

			if (fdFile.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
				va_copy(args_cp, args);
				fn(sPath, args_cp);
				va_end(args_cp);
			}
		}
		while (FindNextFile(hFind, &fdFile));
	}

	va_end(args);

	FindClose(hFind);
}
#elif __linux__

#else

#endif // _WIN32

#define get_db_handle(index) *index

int index_init(TIndex * index, char * path) {
	return sqlite3_open(path, index);
}

void index_free(TIndex *index) {
	sqlite3_close(*index);
}

void index_build(TIndex *index, char *wf_path, char *map_path) {
	THashMap id_map;
	TPreparedStatements stmts;
	sqlite3 *db = get_db_handle(index);

	setup_db(db);
	hashmap_init(&id_map, 7000, &jenkins_oat_hash, &strcmp);
	setup_statements(db, &stmts);

	apply_to_dir(wf_path, &read_workflow, db, &id_map, &stmts);
	apply_to_dir(map_path, &read_mapping, db, &id_map, &stmts);

	hashmap_destroy(&id_map);

	finialize_statements(db, &stmts);
}

TGraph *index_load_graph(TIndex *index, unsigned int id) {
	sqlite3 *db = get_db_handle(index);
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

	return graph;
}

TMatching *index_load_matching(TIndex *index, unsigned int id1, unsigned int id2, size_t vertex_count) {
	sqlite3 *db = get_db_handle(index);
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

		matching[vertex_number_1].value = vertex_number_2;
		matching[vertex_number_1].score = similarity;
	}

	sqlite3_finalize(selectMatchingStatement);

	return matching;
}
