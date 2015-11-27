//
// Created by david on 11/23/2015.
//

#ifndef GRAPHS_ARRAY_H
#define GRAPHS_ARRAY_H

#include <stdlib.h>

typedef struct Array TArray;
typedef int (*array_entry_cmp_fn) (void* el1, void* el2, size_t el_len);

struct Array {
    size_t entry_count;
    size_t entry_length;
    void* values_init_addr; // the initial address the array was initialized with (before subarraying, etc.)
    void* values;
};

void array_init(TArray* array, size_t entry_count, size_t entry_length);
void array_delete(TArray* array);
//void array_set(TArray* array, int index, int value);
#define array_set(A, T, I, V) *((T*)A->values + I) = V
void* array_get(TArray* array, int index);
void array_sort(TArray* array, array_entry_cmp_fn cmp_fn);
int array_find(TArray* array, void* lookup_val, array_entry_cmp_fn cmp_fn);
void array_subarray(TArray* array, size_t start_index, size_t end_index);

#define ARRAY(T) struct { \
    size_t entry_count; \
    T* values; \
}

#define __array_init(A, N) A.entry_count = N; A.values = malloc(sizeof(*(A.values)) * N);
#define __array_set(A, index, value) A.values[index] = value
#define __array_get(A, index) A.values[index]

//#define array_init_g(A, N) array_init(&(A.array), N, )

#endif //GRAPHS_ARRAY_H
