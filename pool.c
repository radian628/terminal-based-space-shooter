#include "dynarray.h"
#include "dynarray_impl.h"
#include "pool.h"

#include <stdlib.h>

// TODO: if enough time make this use a hash table for fast ID lookup
struct pool {
  struct dynarray data;
  struct dynarray alive_list;
  struct dynarray id_list;
  size_t next_id;
  size_t dead_count;
};


typedef struct pool pool;

typedef struct pool_iter pool_iter;

pool *pool_create(size_t element_size) {
  pool *pool = malloc(sizeof(struct pool));
  pool->next_id = 0;
  pool->dead_count = 0;
  da_create_in_place(&pool->data, element_size);
  da_create_in_place(&pool->alive_list, sizeof(uint8_t));
  da_create_in_place(&pool->id_list, sizeof(size_t));
  return pool;
}

struct new_pool_item pool_alloc(pool *p) {
  uint8_t alive = 1;
  da_append(&p->alive_list, &alive);
  da_append(&p->id_list, &p->next_id);
  struct new_pool_item npi;
  npi.id = p->next_id;
  npi.data = da_reserve_at_end(&p->data);

  p->next_id++;

  return npi; 
}

int filter_by_alive(void *element, size_t index, void *context) {
  pool *p = context;

  uint8_t *alive_ptr = da_get_ptr(&p->alive_list, index);

  return *alive_ptr;
}

void pool_try_filter(pool *p) {
  // filter iff more than half are dead 
  if (p->dead_count < da_size(&p->data) / 2) return;

  // filter out dead elements
  da_filter(&p->data, filter_by_alive, p);
  da_filter(&p->id_list, filter_by_alive, p);

  // replace the alive list since we know all items are alive now
  da_free_in_place(&p->alive_list);
  da_create_in_place(&p->alive_list, sizeof(size_t));
  for (size_t i = 0; i < da_size(&p->data); i++) {
    uint8_t alive = 1;
    da_append(&p->alive_list, &alive);
  }

  p->dead_count = 0;
}

// ideally this'd use a hash table
// 1 -> successful free; 0 -> unsuccessful free 
int pool_free_by_id(pool *p, size_t id) {
  for (size_t i = 0; i < da_size(&p->data); i++) {
    size_t *found_id_ptr = da_get_ptr(&p->id_list, i);

    if (*found_id_ptr == id) {
      uint8_t *alive_ptr = da_get_ptr(&p->alive_list, i);
      *alive_ptr = 0;
      p->dead_count++;

      pool_try_filter(p);
      return 1;
    }
  }
  return 0;
}

int pool_item_is_alive(pool *p, size_t i) {
  uint8_t *alive_ptr = da_get_ptr(&p->alive_list, i);
  return *alive_ptr;
}

pool_iter pool_start_iter(pool *p) {
  struct pool_iter pi;
  pi.pool = p;
  pi.index = 0;
  while (pi.index < da_size(&p->data) && !pool_item_is_alive(p, pi.index)) {
    pi.index++;
  }
  return pi;
}

pool_iter pi_next(pool_iter pi) {
  do {
    pi.index++; 
  } while (
    pi.index < da_size(&pi.pool->data) && 
    !pool_item_is_alive(pi.pool, pi.index)
  );
 
  return pi;
}

void *pi_get(pool_iter pi) {
  if (pi.index >= da_size(&pi.pool->data)) return NULL;
  return da_get_ptr(&pi.pool->data, pi.index);
}