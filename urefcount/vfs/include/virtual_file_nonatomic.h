#pragma once

#include <vector>
#include "virtual_file.h"

class virtual_file_nonatomic : public virtual_file {
 private:
  struct alignas(CACHE_LINE_SIZE) PageNonatomic {
    int refcount;
  };
  std::vector<PageNonatomic> pages_nonatomic;

 protected:
  int ref(off_t bn) override;
  int unref(off_t bn) override;

 public:
  virtual_file_nonatomic(const char* path);
  int query(off_t bn) override;
};
