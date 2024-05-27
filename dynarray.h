#pragma once
#include <stddef.h>

struct dynarray;

typedef struct dynarray dynarray;

dynarray *da_create(size_t element_size);
void da_create_in_place(dynarray *da, size_t element_size);
void da_free(dynarray *da);
void da_free_in_place(dynarray *da);
void da_try_resize(dynarray *da);
void da_append(dynarray *da, void *ptr_to_element);
void *da_reserve_at_end(dynarray *da);
void da_filter(dynarray *da, int (*filter)(void *, size_t, void *), void *closure_scope);
void *da_start(dynarray *da);
void *da_end(dynarray *da);
size_t da_size(dynarray *da);
void *da_get_ptr(dynarray *da, size_t i);