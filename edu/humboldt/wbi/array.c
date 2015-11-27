//
// Created by david on 11/23/2015.
//

#include <string.h>
#include "array.h"

void array_init(TArray* array, size_t entry_count, size_t entry_length) {
    array->entry_count = entry_count;
    array->entry_length = entry_length;
    array->values = malloc(entry_count * entry_length);
    array->values_init_addr = array->values;
}

void array_delete(TArray* array) {
    free(array->values_init_addr);
    free(array);
}

void* array_get(TArray* array, int index) {
    return array->values + (array->entry_length * index);
}

void array_subarray(TArray* array, size_t start_index, size_t end_index) {
    // end_index excluded

    size_t offset = array->entry_length * start_index;

    array->values += offset;
    array->entry_count = end_index - start_index;
}

void swap(void* src, void* dst, size_t len) {
    void* tmp = malloc(len);

    memcpy(tmp, src, len);
    memcpy(src, dst, len);
    memcpy(dst, tmp, len);

    free(tmp);
}

void* partition(void* base_index, void* end_index, size_t entry_length, array_entry_cmp_fn cmp_fn) {
    void* i = base_index;

    for (void* j = base_index; j < end_index; j += entry_length) {
        if (cmp_fn(j, end_index, entry_length) <= 0) {
            swap(i, j, entry_length);
            i += entry_length;
        }
    }

    swap(i, end_index, entry_length);

    return i;
}

void quicksort(void* base_index, void* end_index, size_t entry_length, array_entry_cmp_fn cmp_fn) {
    if (base_index < end_index) {
        void* p_index = partition(base_index, end_index, entry_length, cmp_fn);
        quicksort(base_index, p_index - entry_length, entry_length, cmp_fn);
        quicksort(p_index + entry_length, end_index, entry_length, cmp_fn);
    }
}

void array_sort(TArray* array, array_entry_cmp_fn cmp_fn) {
    quicksort(array->values, array->values + (array->entry_length * (array->entry_count - 1)), array->entry_length,
              cmp_fn);
}

int array_find(TArray* array, void* lookup_val, array_entry_cmp_fn cmp_fn) {
    int l = 0;
    int r = array->entry_count - 1;
    int m;

    while (l <= r) {
        m = l + (r - l) / 2;

        int cmp_res = cmp_fn(lookup_val, array->values + (m * array->entry_length), array->entry_length);

        if(cmp_res > 0) {
            l = m + 1;
        }
        else if (cmp_res < 0) {
            r = m - 1;
        }
        else {
            return m;
        }
    }

    return -1;
}
