#pragma once

#include <vector>
#include "params.h"
#include "virtual_file.h"

class virtual_file_cache_affinity : public virtual_file {
 private:
  struct alignas(CACHE_LINE_SIZE) core {
    int *local_refcounts;
  };
  std::vector<core> cores;
  int ncores;

 protected:
  void setup_hook() override;
  int ref(off_t bn) override;
  int unref(off_t bn) override;

 public:
  virtual_file_cache_affinity(const char *path);
  ~virtual_file_cache_affinity();
  int query(off_t bn) override;
};
