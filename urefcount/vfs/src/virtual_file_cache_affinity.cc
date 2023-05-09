#include "virtual_file_cache_affinity.h"
#include "utils.h"

virtual_file_cache_affinity::virtual_file_cache_affinity(const char *path)
    : virtual_file(path) {}

virtual_file_cache_affinity::~virtual_file_cache_affinity() {
  for (auto &c : this->cores) {
    delete[] c.local_refcounts;
  }
}

/**
 * @note
 * Space overhead: O(C * N)
 * - C: Number of cores
 * - N: Number of pages
 */
void virtual_file_cache_affinity::setup_hook() {
  const int id = thread_id;
  size_t space = roundup(this->get_capacity(), CACHE_LINE_SIZE);
  this->cores.emplace_back();
  this->cores[id].local_refcounts = new int[space]{0};
}

/**
 * @note
 * Counting overhead: Low (No cacheline bouncing)
 */
int virtual_file_cache_affinity::ref(off_t bn) {
  int id = this->thread_id;
  int *refcounts = this->cores[id].local_refcounts;
  int old = refcounts[bn];
  refcounts[bn] = old + 1;
  return old;
}

/**
 * @note
 * Counting overhead: Low (No cacheline bouncing)
 */
int virtual_file_cache_affinity::unref(off_t bn) {
  int id = this->thread_id;
  int *refcounts = this->cores[id].local_refcounts;
  int old = refcounts[bn];
  refcounts[bn] = old - 1;
  return old;
}

/**
 * @note
 * Query overhead: O(C)
 * - C: Number of cores
 */
int virtual_file_cache_affinity::query(off_t bn) {
  int refcount = this->pages[bn].refcount;
  for (auto &c : this->cores) {
    refcount += c.local_refcounts[bn];
  }
  return refcount;
}