//
// Created by david on 12/5/2015.
//

#include "index.h"

void index_init(TIndex* index, char* path) {
    FILE* f = fopen(path, "rb");

    int body_path_length;
    char* body_path;
    int entry_count;

    fread(&body_path_length, sizeof(int), 1, f);
    body_path = malloc(sizeof(char) * body_path_length);
    fread(body_path, sizeof(char) * body_path_length, 1, f);
    fread(&entry_count, sizeof(int), 1, f);

    index->header_entries = malloc(entry_count * sizeof(THeaderEntry));
    index->header_tree = malloc(sizeof(TTreeSet));
    index->data_file = fopen(body_path, "rb");

    treeset_init(index->header_tree, 8);

    for (int i = 0; i < entry_count; ++i) {
        fread(&(index->header_entries[i].id), sizeof(int), 1, f);
        fread(&(index->header_entries[i].pos), sizeof(long), 1, f);
        fread(&(index->header_entries[i].size), sizeof(int), 1, f);

        treeset_add(index->header_tree, index->header_entries[i].id, index->header_entries + i);
    }

    fclose(f);
}

void index_delete(TIndex* index) {
    free(index->header_entries);
    fclose(index->data_file);
    treeset_delete(index->header_tree);
}

void* index_get_entry(TIndex* index, int key) {
    THeaderEntry* he = treeset_get(index->header_tree, key);
    void* data = malloc(he->size);

    fseek(index->data_file, he->pos, SEEK_SET);
    fread(data, sizeof(int), he->size / sizeof(int), index->data_file);

    return data;
}
