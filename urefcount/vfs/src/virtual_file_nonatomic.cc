#include "virtual_file_nonatomic.h"

virtual_file_nonatomic::virtual_file_nonatomic(const char *path)
    : virtual_file(path) {

  size_t capacity = this->get_capacity();
  for (long i = 0; i < capacity; i++) {
    this->pages_nonatomic.emplace_back();
  }
}

int virtual_file_nonatomic::ref(off_t bn) {
  return this->pages_nonatomic[bn].refcount++;
}

int virtual_file_nonatomic::unref(off_t bn) {
  return this->pages_nonatomic[bn].refcount--;
}

int virtual_file_nonatomic::query(off_t bn) {
  return this->pages_nonatomic[bn].refcount;
}