#ifndef __VIRTUAL_FILE_REFCACHE_H__
#define __VIRTUAL_FILE_REFCACHE_H__

#include "virtual_file.h"

class virtual_file_refcache : public virtual_file {
 private:
  static constexpr size_t CACHE_SLOTS = 4096;
  struct alignas(CACHE_LINE_SIZE) core {
    long local_epoch;
    struct way {
      page *obj;
      int delta;
    } ways[CACHE_SLOTS];
  };
  long global_epoch;         // atomic?
  size_t global_epoch_left;  // atomic
  core *cores;
  int ncores;

 protected:
  int ref(off_t bn) override;
  int unref(off_t bn) override;

 public:
  virtual_file_refcache(const char *path);
  virtual_file_refcache(const char *path, int ncores = 1);
  ~virtual_file_refcache();
  int query(off_t bn) override;
};

#endif /* __VIRTUAL_FILE_REFCACHE_H__ */