
#include "virtual_file_refcache.h"
#include <assert.h>
#include <cpu.h>
#include <stdlib.h>
#include "utils.h"

long global_epoch;         // atomic?
size_t global_epoch_left;  // atomic

virtual_file_refcache::virtual_file_refcache(const char *path)
    : virtual_file(path), ncores(8) {
  this->cores = new core[this->ncores];
  for (int i = 0; i < this->ncores; i++) {
    this->cores[i].local_epoch = 0;
    this->cores[i].NCPU = this->ncores;
  }
}
virtual_file_refcache::virtual_file_refcache(const char *path,
                                                         int ncores)
    : virtual_file(path), ncores(ncores) {
  this->cores = new core[this->ncores];
  for (int i = 0; i < this->ncores; i++) {
    this->cores[i].local_epoch = 0;
    this->cores[i].NCPU = this->ncores;    
  }
}

virtual_file_refcache::~virtual_file_refcache() {
  delete[] this->cores;  
}



void virtual_file_refcache::core::evict(struct way *way, bool local_epoch_is_exact) {
  page *obj = way->obj;
  auto delta = way->delta;

  //lock point
  way->delta = 0;
  way->obj = nullptr;

  if ((obj->_refcount += delta) == 0) {
    if (obj->review_epoch == 0) {
      obj->review_epoch = local_epoch + (local_epoch_is_exact ? 2 : 3);
      obj->dirty = false;
      review_list.push_back(obj);
    } else {
      obj->dirty = true;
    }
  }
}

void virtual_file_refcache::core::review() {
  long epoch = global_epoch;
  int last_reviewable = -1;

  for (page *obj : review_list) {
    if (obj->review_epoch > epoch)
      break;
    last_reviewable++;
  }

  if(!last_reviewable)
    return;
  
  std::list<page *> reviewable;
  for (auto node : review_list) {
    reviewable.push_back(node);
    review_list.pop_front();
  }

  auto review = reviewable.begin();
  while (review != reviewable.end()) {
    auto obj = *review;
    if (obj->_refcount == 0) {
      if (obj->dirty) {
        obj->dirty = false;
        obj->review_epoch = epoch + 2;
        review_list.push_back(obj);
      } else {
        obj->review_epoch = 0;
        reap_list.push_back(obj);
      }
    } else {
      obj->review_epoch = 0;
    }
    ++review;
  }
}

void virtual_file_refcache::core::flush() {
  long cur_global = global_epoch;
  if (cur_global == local_epoch) {
    return;
  }
  
  local_epoch = cur_global;

  for(int i = 0; i < CACHE_SLOTS; i++) {
    if(ways[i].obj)
      evict(&ways[i], true);
  }

  if(--global_epoch_left == 0) {
    global_epoch_left = NCPU;
    ++global_epoch;
  }
}

int virtual_file_refcache::ref(off_t bn) {
  int cpuid = get_cpuid();
  page *obj = &(this->pages[bn]);
  auto way = cores[cpuid].get_way(obj);
  way->delta++;  
}

int virtual_file_refcache::unref(off_t bn) {
  int cpuid = get_cpuid();
  page *obj = &(this->pages[bn]);
  auto way = cores[cpuid].get_way(obj);
  way->delta--;  
}


// fake true refcount.
int virtual_file_refcache::query(off_t bn) {

  int count = 0;  
  page *obj = &(this->pages[bn]);
  for (int i = 0; i < ncores; i++) {
    auto way = cores[i].hash_way(obj);
    if (way->obj == obj) {
      count += way->delta;
    }
  }
  count += obj->_refcount;

  return count;
}

