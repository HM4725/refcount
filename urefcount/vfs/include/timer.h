#pragma once

#include <atomic>
#include <chrono>
#include <cstdio>
#include <functional>
#include <thread>

class Timer {
 public:
  ~Timer() {
    if (mRunning) {
      stop();
    }
  }
  typedef std::chrono::milliseconds Interval;
  typedef std::function<void(void)> Timeout;

  void start(const Interval &interval, const Timeout &timeout) {
    mRunning = true;

    mThread = std::thread([this, interval, timeout] {
      while (mRunning) {
        std::this_thread::sleep_for(interval);

        timeout();
      }
    });
  }
  void stop() {
    mRunning = false;
    mThread.join();
  }

 private:
  std::thread mThread{};
  std::atomic_bool mRunning{};
};
