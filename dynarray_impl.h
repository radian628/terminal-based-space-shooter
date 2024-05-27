#pragma once

struct dynarray {
  size_t size;
  size_t capacity;
  char *data;
  size_t element_size;
};