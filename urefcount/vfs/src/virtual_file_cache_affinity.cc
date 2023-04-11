#include "virtual_file_cache_affinity.h"
#include <assert.h>
#include <cpu.h>
#include <stdlib.h>
#include "utils.h"

/**
 * @note
 * Space overhead: O(C * N)
 * - C: Number of cores
 * - N: Number of pages
 */
virtual_file_cache_affinity::virtual_file_cache_affinity(const char *path)
    : virtual_file(path), ncores(8) {
  this->cores = new core[this->ncores];
  size_t space = roundup(this->get_capacity(), this->CACHE_LINE_SIZE);
  for (int i = 0; i < this->ncores; i++) {
    this->cores[i].local_refcounts = new int[space]{0};
  }
}

/**
 * @note
 * Space overhead: O(C * N)
 * - C: Number of cores
 * - N: Number of pages
 */
virtual_file_cache_affinity::virtual_file_cache_affinity(const char *path,
                                                         int ncores)
    : virtual_file(path), ncores(ncores) {
  this->cores = new core[this->ncores];
  size_t space = roundup(this->get_capacity(), this->CACHE_LINE_SIZE);
  for (int i = 0; i < this->ncores; i++) {
    this->cores[i].local_refcounts = new int[space]{0};
  }
}

virtual_file_cache_affinity::~virtual_file_cache_affinity() {
  for (int i = 0; i < this->ncores; i++) {
    delete[] this->cores[i].local_refcounts;
  }
  delete[] this->cores;
}

/**
 * @note
 * Counting overhead: Low (No cacheline bouncing)
 */
int virtual_file_cache_affinity::ref(off_t bn) {
  // WARN: Core of the thread must be fixed!
  int cpuid = get_cpuid();
  int *refcounts = this->cores[cpuid].local_refcounts;
  int old = refcounts[bn];
  refcounts[bn] = old + 1;
  return old;
}

/**
 * @note
 * Counting overhead: Low (No cacheline bouncing)
 */
int virtual_file_cache_affinity::unref(off_t bn) {
  // WARN: Core of the thread must be fixed!
  int cpuid = get_cpuid();
  int *refcounts = this->cores[cpuid].local_refcounts;
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
  int refcount = this->pages[bn]._refcount;
  for (int i = 0; i < this->ncores; i++) {
    int *refcounts = this->cores[i].local_refcounts;
    refcount += refcounts[bn];
  }
  return refcount;
}