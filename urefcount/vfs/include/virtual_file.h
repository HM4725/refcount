#pragma once

#include <sys/types.h>
#include <atomic>
#include <mutex>
#include <vector>
#include "params.h"
#include "thread_manager.h"

class virtual_file : public thread_manager {
 private:
  // Private Types
  static constexpr size_t DEFAULT_BLOCK_NUM = 4;
  struct alignas(BLOCK_SIZE) block {
    char data[BLOCK_SIZE];
  };
  // Private Fields
  int fd;
  std::vector<block> buffer;
  // Private Methods
  ssize_t read_block(void* buf, size_t nbytes, off_t offset);

 protected:
  // Protected Types
  struct alignas(CACHE_LINE_SIZE) page {
    std::atomic<int> refcount;
    bool active;
    bool dirty;
    long reserved;
    std::mutex m;
    page() : refcount(0), active(false), dirty(false) {}
    page(page&& p) : refcount(0), active(false), dirty(false) {}
  };
  // Protected Fields
  std::vector<page> pages;
  // Protected Methods
  virtual int ref(off_t bn);
  virtual int unref(off_t bn);

 public:
  // Public Methods
  virtual_file(const char* fname);
  virtual ~virtual_file();
  size_t get_capacity() const;
  ssize_t read(void* buf, size_t nbytes, off_t offset);
  virtual int query(off_t bn);
};
