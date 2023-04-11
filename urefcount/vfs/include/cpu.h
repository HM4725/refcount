// _GNU_SOURCE
#ifndef __CPU_H__
#define __CPU_H__

#include <pthread.h>
#include <sched.h>

inline int get_cpuid() { return sched_getcpu(); }

inline void set_cpuid(int id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(id, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}

#endif /* __CPU_H__ */