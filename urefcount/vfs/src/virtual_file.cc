
#include "virtual_file.h"
#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

virtual_file::virtual_file(const char *path) {
  int fd = open(path, O_RDWR | O_CREAT | O_SYNC, 0644);
  this->fd = fd;
  off_t size = lseek(fd, 0, SEEK_END);
  if (size == 0) {
    ftruncate(fd, this->BLOCK_SZ * this->BLOCK_DEFAULT_NUM);
    this->capacity = this->BLOCK_DEFAULT_NUM;
  } else {
    this->capacity = (size + this->BLOCK_SZ - 1) / this->BLOCK_SZ;
  }
  this->cache = new block[this->capacity];
  this->pages = new page[this->capacity];
}

virtual_file::~virtual_file() {
  delete[] this->pages;
  delete[] this->cache;
  close(this->fd);
}

ssize_t virtual_file::read(void *buf, size_t nbytes, off_t offset) {
  // caching
  off_t bn_begin = offset / this->BLOCK_SZ;
  off_t bn_end = (offset + nbytes + this->BLOCK_SZ - 1) / this->BLOCK_SZ;
  for (off_t bn = bn_begin; bn < bn_end; bn++) {
    if (this->pages[bn].cached == false) {
      pread(this->fd, &this->cache[bn], this->BLOCK_SZ, bn * this->BLOCK_SZ);
      this->pages[bn].cached = true;
    }
  }

  // read blocks
  ssize_t res = 0;
  char *_buf = (char *)buf;
  off_t fp = offset;
  while (fp < offset + nbytes) {
    size_t _nbytes = min(this->BLOCK_SZ - (fp % this->BLOCK_SZ), nbytes - res);
    ssize_t res_block = read_block(_buf, _nbytes, fp);
    _buf += res_block;
    fp += res_block;
    res += res_block;
  }
  return res;
}

ssize_t virtual_file::read_block(void *buf, size_t nbytes, off_t offset) {
  off_t bn = offset / this->BLOCK_SZ;
  off_t off = offset % this->BLOCK_SZ;
  assert(nbytes <= this->BLOCK_SZ);
  assert(bn < this->capacity);

  this->ref(bn);
  memcpy(buf, &this->cache[bn].d[off], nbytes);
  this->unref(bn);
  return nbytes;
}

size_t virtual_file::get_capacity() const {
  return this->capacity;
}

int virtual_file::ref(off_t bn) {
  int old = __sync_fetch_and_add(&this->pages[bn]._refcount, 1);
  return old;
}

int virtual_file::unref(off_t bn) {
  int old = __sync_fetch_and_sub(&this->pages[bn]._refcount, 1);
  return old;
}

int virtual_file::query(off_t bn) {
  return this->pages[bn]._refcount;
}
