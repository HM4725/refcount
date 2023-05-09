#include <cxxabi.h>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <typeinfo>
#include "virtual_file_types.h"

#define DEFAULT_RUNTIME 1

/**
 * Controller
 * Multi-Producer, Single-Consumer
 */
class Controller {
 private:
  int nProducers;
  int nReady;
  bool finish;
  std::mutex m;
  std::condition_variable producers_cv;
  std::condition_variable consumer_cv;

 public:
  // Producers
  void ready() {
    std::unique_lock lk(m);
    nReady += 1;
    if (nProducers == nReady) {
      consumer_cv.notify_one();
    }
    producers_cv.wait(lk);
    lk.unlock();
  }
  bool work() { return !finish; }

  // Consumer
  void start() {
    std::unique_lock lk(m);
    while (nProducers != nReady) {
      consumer_cv.wait(lk);
    }
    lk.unlock();
    producers_cv.notify_all();
  }
  void wait(int time) {
    std::this_thread::sleep_for(std::chrono::seconds(time));
  }
  void stop() { finish = true; }

  Controller() = delete;
  Controller(int N) : nProducers(N), nReady(0), finish(false) {}
};

/**
 * Thread routine: DRBH
 */
template <class T>
long DRBH(const size_t N, const int time = DEFAULT_RUNTIME) {
  std::vector<std::thread> threads;
  Controller controller(N);
  std::vector<long> res;
  T file_manager("bench.db");

  for (long i = 0; i < N; i++) {
    res.emplace_back(0);
    threads.emplace_back([&file_manager, &controller, &res]() {
      int thread_id = file_manager.setup();
      char buffer[BLOCK_SIZE];
      long iter = 0;

      controller.ready();

      while (controller.work()) {
        file_manager.read(buffer, BLOCK_SIZE, 0);
        iter += 1;
      }
      res[thread_id] = iter;
    });
  }

  controller.start();
  controller.wait(time);
  controller.stop();
  for (auto &th : threads) {
    th.join();
  }
  return std::accumulate(res.begin(), res.end(), 0);
}

/**
 * Benchmark
 */
template <class T>
void bench(const size_t N, std::ofstream &logger) {
  int status;
  char *type_str = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
  long throughput = DRBH<T>(N);
  std::cout << N << ',' << throughput << "," << type_str << std::endl;
  logger << N << ',' << throughput << "," << type_str << std::endl;
}

int main() {
  std::ofstream logger;
  logger.open("log.csv");

  const unsigned int NCORES = std::thread::hardware_concurrency();

  logger << "N,iters,type" << std::endl;
  std::cout << "N,iters,type" << std::endl;

  for (int n = 1; n <= NCORES; n++) {
    bench<virtual_file>(n, logger);
    bench<virtual_file_noop>(n, logger);
    bench<virtual_file_nonatomic>(n, logger);
    bench<virtual_file_cache_affinity>(n, logger);
    bench<virtual_file_refcache>(n, logger);
  }

  logger.close();
  return 0;
}
