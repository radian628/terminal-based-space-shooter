#pragma once

#include "pool.h"

void test_pool() {
  printf("testing pool!\n");
  pool *pool = pool_create(sizeof(char *));

  new_pool_item i1 = pool_alloc(pool);
  *(char **)(i1.data) = "hello";
  new_pool_item i2 = pool_alloc(pool);
  *(char **)(i2.data) = "world";
  new_pool_item i3 = pool_alloc(pool);
  *(char **)(i3.data) = "!";
  new_pool_item i4 = pool_alloc(pool);
  *(char **)(i4.data) = "bye!";

  for (pool_iter i = pool_start_iter(pool); pi_get(i); i = pi_next(i)) {
    printf("%s\n", *(char **)pi_get(i));
  }

  pool_free_by_id(pool, i2.id);
  pool_free_by_id(pool, i3.id);

  for (pool_iter i = pool_start_iter(pool); pi_get(i); i = pi_next(i)) {
    printf("%s\n", *(char **)pi_get(i));
  }
}