#include "virtual_file_nonatomic.h"

int virtual_file_nonatomic::ref(off_t bn) {
  int old = this->pages[bn]._refcount;
  this->pages[bn]._refcount = old + 1;
  return old;
}

int virtual_file_nonatomic::unref(off_t bn) {
  int old = this->pages[bn]._refcount;
  this->pages[bn]._refcount = old - 1;
  return old;
}

int virtual_file_nonatomic::query(off_t bn) {
  return this->pages[bn]._refcount;
}