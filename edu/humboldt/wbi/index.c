//
// Created by david on 12/5/2015.
//

#include "index.h"
#include "quadtree.h"

void index_init(TIndex* index, char* header_path, char* body_path) {
    FILE* f = fopen(header_path, "rb");

    int entry_count;
    int key_dim;

    fread(&entry_count, sizeof(int), 1, f);
    fread(&key_dim, sizeof(int), 1, f);

    index->header_entry_size = sizeof(long) + (key_dim + 1) * sizeof(int);
    index->header_entries = malloc(entry_count * index->header_entry_size);
    index->data_file = fopen(body_path, "rb");

    // not very pretty; perhaps move to dim-indexed array...
    if (key_dim == 1) {
        index->header_tree = malloc(sizeof(TTreeSet));
        index->init_fn = treeset_init;
        index->add_fn = treeset_add;
        index->get_fn = treeset_get;
		index->has_fn = treeset_has;
    }
    else if (key_dim == 2) {
        index->header_tree = malloc(sizeof(TQuadTree));
        index->init_fn = quadtree_init;
        index->add_fn = quadtree_add;
        index->get_fn = quadtree_get;
		index->has_fn = quadtree_has;
    }
    else {
        exit(1);    // a dimension we can't handle
    }

    index->init_fn(index->header_tree);

    for (int i = 0; i < entry_count; ++i) {
        THeaderEntry *headerEntry = index->header_entries + index->header_entry_size * i;

        for (int j = 0; j < key_dim; ++j) {
            fread(&(headerEntry->id[j]), sizeof(int), 1, f);
        }

        fread(&(headerEntry->pos), sizeof(long), 1, f);
        fread(&(headerEntry->size), sizeof(int), 1, f);

        index->add_fn(index->header_tree, headerEntry, headerEntry->id);
    }

    fclose(f);
}

void index_delete(TIndex* index) {
    free(index->header_entries);
    fclose(index->data_file);
}

int index_has(TIndex* index, int *key) {
	return index->has_fn(index->header_tree, key);
}

void *index_get(TIndex *index, int *key) {
    THeaderEntry *he = index->get_fn(index->header_tree, key);

	if(he != NULL) {
		void* data = malloc(he->size);

	    fseek(index->data_file, he->pos, SEEK_SET);
	    fread(data, sizeof(int), he->size / sizeof(int), index->data_file);

	    return data;
	}
	else {
		return NULL;
	}
}
