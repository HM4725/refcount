
#include "virtual_file_refcache.h"
#include <assert.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <thread>
#include "utils.h"

void virtual_file_refcache::setup_hook() {
  const int id = thread_id;
  global_epoch_left.fetch_add(1);
  this->cores.emplace_back();
}

virtual_file_refcache::way *virtual_file_refcache::hash_way(int id,
                                                            object *obj) {
  core *c = &this->cores[id];
  unsigned long wayno = (unsigned long)obj;
  wayno ^= (wayno >> 32) ^ (wayno >> 20) ^ (wayno >> 12);
  wayno ^= (wayno >> 7) ^ (wayno >> 4);
  wayno %= CACHE_SLOTS;
  return &c->ways[wayno];
}

virtual_file_refcache::way *virtual_file_refcache::get_way(int id,
                                                           object *obj) {
  way *way = hash_way(id, obj);
  if (way->obj != obj) {
    if (way->obj) {
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

virtual_file_refcache::virtual_file_refcache(const char *path)
    : virtual_file(path), global_epoch(1), global_epoch_left(0) {
  for (long i = 0; i < this->get_capacity(); i++) {
    this->objs.emplace_back();
  }
  this->timer.start(std::chrono::milliseconds(10), [this]() {
    if (this->global_epoch_left == 0) {
      this->global_epoch_left = this->total_thread_num;
      this->global_epoch += 1;
    }
    for (auto &c : this->cores) {
      c.tick = true;
    }
  });
}

virtual_file_refcache::~virtual_file_refcache() {
  // thread_join
  this->timer.stop();
}

void virtual_file_refcache::evict(way *way, bool local_epoch_is_exact) {
  object *obj = way->obj;
  int delta = way->delta;
  core *c = &this->cores[this->thread_id];

  std::lock_guard<std::mutex> l(obj->lock);
  way->delta = 0;
  way->obj = nullptr;

  if ((obj->refcount += delta) == 0) {
    if (obj->review_epoch == 0) {
      obj->review_epoch = c->local_epoch + (local_epoch_is_exact ? 2 : 3);
      obj->dirty = false;
      c->review_list.push_back(obj);
    } else {
      obj->dirty = true;
    }
  }
}

void virtual_file_refcache::review() {
  long epoch = this->global_epoch;
  core *c = &this->cores[this->thread_id];
  std::list<object *>::iterator it;

  for (it = c->review_list.begin(); it != c->review_list.end(); it++) {
    object *obj = *it;
    if (obj->review_epoch > epoch) break;
  }

  if (it == c->review_list.begin()) {
    return;
  }

  std::list<object *> reviewable;
  reviewable.splice(reviewable.begin(), c->review_list, c->review_list.begin(),
                    it);
  for (object *obj : reviewable) {
    std::lock_guard<std::mutex> l(obj->lock);
    if (obj->refcount == 0) {
      if (obj->dirty) {
        obj->dirty = false;
        obj->review_epoch = epoch + 2;
        c->review_list.push_back(obj);
      } else {
        obj->review_epoch = 0;
        c->reap_list.push_back(obj);
      }
    } else {
      obj->review_epoch = 0;
    }
  }
}

void virtual_file_refcache::flush() {
  core *c = &this->cores[this->thread_id];
  if (c->local_epoch == this->global_epoch) {
    return;
  }

  for (int i = 0; i < CACHE_SLOTS; i++) {
    if (c->ways[i].obj) evict(&c->ways[i], true);
  }
  global_epoch_left.fetch_sub(1);
  c->local_epoch = this->global_epoch;
}

int virtual_file_refcache::ref(off_t bn) {
  core *c = &this->cores[this->thread_id];
  if (c->tick) {
    this->flush();
    this->review();
    c->tick = false;
  }

  object *obj = &(this->objs[bn]);
  way *way = get_way(this->thread_id, obj);
  return way->delta++;
}

int virtual_file_refcache::unref(off_t bn) {
  object *obj = &(this->objs[bn]);
  way *way = get_way(this->thread_id, obj);
  return way->delta--;
}

int virtual_file_refcache::query(off_t bn) {
  int count = 0;
  object *obj = &(this->objs[bn]);
  for (int i = 0; i < this->total_thread_num; i++) {
    way *way = hash_way(i, obj);
    if (way->obj == obj) {
      count += way->delta;
    }
  }
  count += obj->refcount;
  return count;
}
