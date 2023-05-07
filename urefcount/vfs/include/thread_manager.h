#pragma once

#include <atomic>
#include <mutex>

class thread_manager {
 private:
  std::mutex m;

 protected:
  static thread_local int thread_id;
  size_t total_thread_num;
  virtual void setup_hook() { void(0); }

 public:
  int setup() {
    m.lock();
    this->thread_id = total_thread_num++;
    this->setup_hook();
    m.unlock();
    return this->thread_id;
  }
  thread_manager() : total_thread_num(0) {}
};
