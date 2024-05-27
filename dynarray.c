
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dynarray.h"
#include "dynarray_impl.h"

dynarray *da_create(size_t element_size) {
  dynarray *da = malloc(sizeof(dynarray));
  da_create_in_place(da, element_size);
  return da;
}

void da_create_in_place(dynarray *da, size_t element_size) {
  da->size = 0;
  da->capacity = 2;
  da->element_size = element_size;
  da->data = malloc(da->element_size * 2);
}

void da_free(dynarray *da) {
  free(da->data);
  free(da);
}

void da_free_in_place(dynarray *da) {
  free(da->data);
}

// run this BEFORE changing the size
void da_try_resize(dynarray *da) {
  if (da->size < da->capacity) return;

  da->data = realloc(da->data, da->element_size * da->capacity * 2);

  if (da->capacity == 0) {
    da->capacity = 1;
  } else {
    da->capacity *= 2;
  }
  
  return;
}

void da_append(dynarray *da, void *ptr_to_element) {
  da_try_resize(da);

  memcpy(da->data + da->size * da->element_size, ptr_to_element, da->element_size);

  da->size++;
}

void *da_reserve_at_end(dynarray *da) {
  da_try_resize(da);
  da->size++;
  return da->data + da->element_size * (da->size - 1);
}

// filter function has two params:
// 1: pointer to element, 2: closure scope
void da_filter(dynarray *da, int (*filter)(void *, size_t, void *), void *closure_scope) {
  int64_t shift_by = 0;

  for (int i = 0; i < da->size; i++) {
    
    // shift elements backwards due to deleted elements
    if (shift_by != 0) {
      memcpy(
        da->data + (i - shift_by) * da->element_size,
        da->data + i * da->element_size,
        da->element_size
      );
    }

    // check if element should be filtered out
    int result = filter(da->data + i * da->element_size, i, closure_scope);
    if (!result) {
      shift_by--;
    }
  }

  // decrease size
  da->size += shift_by;

  // reallocate now that we've filtered out a bunch of elements 
  da->data = realloc(da->data, da->element_size * da->size);
}

void *da_start(dynarray *da) {
  return da->data;
}

void *da_end(dynarray *da) {
  return da->data + da->element_size * da->size;
}

size_t da_size(dynarray *da) {
  return da->size;
}

void *da_get_ptr(dynarray *da, size_t i) {
  return da->data + da->element_size * i;
}