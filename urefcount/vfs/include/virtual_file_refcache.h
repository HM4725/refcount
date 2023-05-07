#pragma once

#include <atomic>
#include <climits>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include "timer.h"
#include "virtual_file.h"

class virtual_file_refcache : public virtual_file {
 private:
  struct alignas(CACHE_LINE_SIZE) object {
    std::atomic<int> refcount;
    bool active;
    bool dirty;
    long review_epoch;
    std::mutex lock;
    object() : refcount(0) {}
    object(object &&other) : refcount(0) {}
  };

  struct way {
    object *obj;
    int delta;
    way() : obj(nullptr), delta(0) {}
  };

  static constexpr size_t CACHE_SLOTS = 4096;
  struct alignas(CACHE_LINE_SIZE) core {
    long local_epoch;
    std::list<object *> review_list;  // Should be circular-queue
    std::list<object *> reap_list;
    way ways[CACHE_SLOTS];
    bool tick;
  };

  std::vector<core> cores;
  std::vector<object> objs;
  long global_epoch;
  std::atomic<size_t> global_epoch_left;
  Timer timer;

  way *hash_way(int id, object *obj);
  way *get_way(int id, object *obj);
  void evict(way *way, bool local_epoch_is_exact);
  void flush();
  void review();

 protected:
  void setup_hook() override;
  int ref(off_t bn) override;
  int unref(off_t bn) override;

 public:
  virtual_file_refcache(const char *path);
  ~virtual_file_refcache();
  int query(off_t bn) override;
};
