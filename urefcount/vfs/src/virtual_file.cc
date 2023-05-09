
#include "virtual_file.h"
#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "params.h"
#include "utils.h"

virtual_file::virtual_file(const char *path) {
  int fd = open(path, O_RDWR | O_CREAT | O_SYNC, 0644);
  size_t capacity;
  this->fd = fd;
  off_t size = lseek(fd, 0, SEEK_END);
  if (size == 0) {
    ftruncate(fd, BLOCK_SIZE * this->DEFAULT_BLOCK_NUM);
    capacity = this->DEFAULT_BLOCK_NUM;
  } else {
    capacity = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
  }
  for (long i = 0; i < capacity; i++) {
    this->buffer.emplace_back();
    this->pages.emplace_back();
  }
}

virtual_file::~virtual_file() { close(this->fd); }

ssize_t virtual_file::read(void *buf, size_t nbytes, off_t offset) {
  // caching
  off_t bn_begin = offset / BLOCK_SIZE;
  off_t bn_end = (offset + nbytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
  for (off_t bn = bn_begin; bn < bn_end; bn++) {
    if (this->pages[bn].active == false) {
      pread(this->fd, &this->buffer[bn].data, BLOCK_SIZE, bn * BLOCK_SIZE);
      this->pages[bn].active = true;
    }
  }

  // read blocks
  ssize_t res = 0;
  char *_buf = (char *)buf;
  off_t fp = offset;
  while (fp < offset + nbytes) {
    size_t _nbytes = min(BLOCK_SIZE - (fp % BLOCK_SIZE), nbytes - res);
    ssize_t res_block = read_block(_buf, _nbytes, fp);
    _buf += res_block;
    fp += res_block;
    res += res_block;
  }
  return res;
}

ssize_t virtual_file::read_block(void *buf, size_t nbytes, off_t offset) {
  off_t bn = offset / BLOCK_SIZE;
  off_t off = offset % BLOCK_SIZE;
  assert(nbytes <= BLOCK_SIZE);
  assert(bn < this->buffer.size());

  this->ref(bn);
  memcpy(buf, &this->buffer[bn].data[off], nbytes);
  this->unref(bn);
  return nbytes;
}

size_t virtual_file::get_capacity() const { return this->buffer.size(); }

int virtual_file::ref(off_t bn) {
  return this->pages[bn].refcount.fetch_add(1);
}

int virtual_file::unref(off_t bn) {
  return this->pages[bn].refcount.fetch_sub(1);
}

int virtual_file::query(off_t bn) { return this->pages[bn].refcount.load(); }
