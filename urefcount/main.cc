#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include "cpu.h"
#include "virtual_file.h"

#define THREADS_BOUND 128
#define VIRTUAL_FILE_TYPE virtual_file
/**
 * Controller
 */
struct {
  bool ready[THREADS_BOUND];
  bool start;
  bool stop;

  void clear() {
    for (int i = 0; i < THREADS_BOUND; i++) {
      this->ready[i] = false;
    }
    this->start = false;
    this->stop = false;
  }
  bool is_all_ready(int n) {
    for (int i = 0; i < n; i++) {
      if (this->ready[i] == false) return false;
    }
    return true;
  }
} controller;

/**
 * Shared file
 */
virtual_file *file = nullptr;

/**
 * Thread routine: DRBH
 */
void *routine_reader(void *arg) {
  long id = (long)arg;
  long iter = 0;
  char buffer[BLOCK_SIZE];
  set_cpuid(id);

  controller.ready[id] = true;
  while (controller.start == false)
    ;

  while (controller.stop == 0) {
    file->read(buffer, BLOCK_SIZE, 0);
    iter += 1;
  }

  return (void *)iter;
}

/**
 * Main
 */
int main() {
  const char *file_path = "bench.db";
  const char *log_path = "log.csv";
  FILE *log = fopen(log_path, "w");

  pthread_t threads[THREADS_BOUND];
  long thread_returns[THREADS_BOUND];
  const unsigned int NCORES = std::thread::hardware_concurrency();

  fprintf(stdout, "N,iters\n");
  fprintf(log, "N,iters\n");
  file = new VIRTUAL_FILE_TYPE(file_path);

  for (int n = 1; n <= NCORES; n++) {
    controller.clear();
    for (long i = 0; i < n; i++) {
      pthread_create(&threads[i], NULL, routine_reader, (void *)i);
    }
    while (controller.is_all_ready(n) == false)
      ;
    controller.start = true;
    sleep(5);
    controller.stop = true;
    for (int i = 0; i < n; i++) {
      pthread_join(threads[i], (void **)&thread_returns[i]);
    }
    fprintf(stdout, "%d", n);
    fprintf(log, "%d", n);

    long sum = 0;
    for (int i = 0; i < n; i++) {
      fprintf(stdout, ",%ld", thread_returns[i]);
      fprintf(log, ",%ld", thread_returns[i]);
      sum += thread_returns[i];
    }
    fprintf(stdout, " -> %ld\n", sum);
    fprintf(log, "\n");
  }
  fprintf(stdout, "_refcount: %d\n", file->query(0));

  fclose(log);
  delete file;
  remove(file_path);
  return 0;
}
