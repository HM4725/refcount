#pragma once

#include "virtual_file.h"

class virtual_file_noop : public virtual_file {
 protected:
  int ref(off_t bn) override;
  int unref(off_t bn) override;

 public:
  virtual_file_noop(const char *path) : virtual_file(path) {}
  int query(off_t bn) override;
};
