#include "virtual_file_noop.h"

inline void noop() { ((void)0); }

int virtual_file_noop::ref(off_t bn) {
  noop();
  return 0;
}

int virtual_file_noop::unref(off_t bn) {
  noop();
  return 0;
}

int virtual_file_noop::query(off_t bn) {
  noop();
  return 0;
}