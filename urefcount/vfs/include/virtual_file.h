#ifndef __VIRTUAL_FILE_H__
#define __VIRTUAL_FILE_H__

#include <sys/types.h>
#include <string.h>

#define BLOCK_SIZE 4096

class virtual_file {
 private:
  static constexpr size_t BLOCK_SZ = 4096;
  static constexpr size_t BLOCK_DEFAULT_NUM = 4;
  struct alignas(BLOCK_SZ) block {
    char d[BLOCK_SZ];
    block() { memset(d, 0, BLOCK_SZ); }
  };

  int fd;
  size_t capacity;  // The number of blocks
  struct block *cache;

 private:
  ssize_t read_block(void *buf, size_t nbytes, off_t offset);

 protected:
  static constexpr size_t CACHE_LINE_SIZE = 64;
  struct alignas(CACHE_LINE_SIZE) page {
    volatile int _refcount;
    bool cached;
    page(): _refcount(0), cached(false) {}
  };

  struct page *pages;

  virtual int ref(off_t bn);
  virtual int unref(off_t bn);

 public:
  virtual_file(const char *fname);
  ~virtual_file();
  ssize_t read(void *buf, size_t nbytes, off_t offset);
  virtual int query(off_t bn);
};

#endif /* __VIRTUAL_FILE_H__ */