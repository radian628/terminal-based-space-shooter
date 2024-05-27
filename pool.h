#pragma once

#include <stddef.h>
#include <stdint.h>

struct pool;
typedef struct pool pool;

struct pool_iter {
  struct pool *pool;
  size_t index;
};
typedef struct pool_iter pool_iter;

struct new_pool_item {
  size_t id;
  void *data;
};
typedef struct new_pool_item new_pool_item;

pool *pool_create(size_t element_size);
new_pool_item pool_alloc(pool *p);
int pool_free_by_id(pool *p, size_t id);
pool_iter pool_start_iter(pool *p);
pool_iter pi_next(pool_iter pi);
void *pi_get(pool_iter pi);
