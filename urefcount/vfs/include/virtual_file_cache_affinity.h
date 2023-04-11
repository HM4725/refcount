#ifndef __VIRTUAL_FILE_CACHE_AFFINITY_H__
#define __VIRTUAL_FILE_CACHE_AFFINITY_H__

#include "virtual_file.h"

class virtual_file_cache_affinity : public virtual_file {
 private:
  struct alignas(CACHE_LINE_SIZE) core {
    int *local_refcounts;
  };
  core *cores;
  int ncores;

 protected:
  int ref(off_t bn) override;
  int unref(off_t bn) override;

 public:
  virtual_file_cache_affinity(const char *path);
  virtual_file_cache_affinity(const char *path, int ncores);
  ~virtual_file_cache_affinity();
  int query(off_t bn) override;
};

#endif /* __VIRTUAL_FILE_CACHE_AFFINITY_H__ */