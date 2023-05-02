#ifndef __VIRTUAL_FILE_REFCACHE_H__
#define __VIRTUAL_FILE_REFCACHE_H__

#include "virtual_file.h"
#include <climits>
#include <list>

class virtual_file_refcache : public virtual_file {
 private:
  static constexpr size_t CACHE_SLOTS = 4096;
  struct alignas(CACHE_LINE_SIZE) core {
    long local_epoch;
    std::list<page *> review_list;
    std::list<page *> reap_list;

    struct way {
      page *obj;
      int delta;
    } ways[CACHE_SLOTS];

    way *hash_way(page *obj) {
      unsigned long wayno = (unsigned long)obj;
      wayno ^= (wayno >> 32) ^ (wayno >> 20) ^ (wayno >> 12);
      wayno ^= (wayno >> 7) ^ (wayno >> 4);
      wayno %= CACHE_SLOTS;
      struct way *way = &ways[wayno];

      return way;
    }

    way *get_way(page *obj) {
      struct way *way = hash_way(obj);
      if (way->obj != obj) {
        if(way->obj) {
          evict(way, false);
        }
        way->obj = obj;
      }
      if (way->delta == INT_MAX || way->delta == INT_MIN) {
        evict(way, false);
        way->obj = obj;
      }
      return way;
    }

    void evict(struct way *way, bool local_epoch_is_exact);

    void flush();

    void review();

  };
  // long global_epoch;         // atomic?
  // size_t global_epoch_left;  // atomic
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