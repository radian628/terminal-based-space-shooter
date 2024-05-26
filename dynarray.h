struct dynarray;

typedef struct dynarray dynarray;

dynarray *da_create(size_t element_size);
void da_free(dynarray *da);
void da_try_resize(dynarray *da);
void da_append(dynarray *da, void *ptr_to_element);
void da_filter(dynarray *da, int (*filter)(void *, void *), void *closure_scope);
void *da_start(dynarray *da);
void *da_end(dynarray *da);
size_t da_size(dynarray *da);