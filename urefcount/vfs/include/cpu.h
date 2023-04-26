#ifndef __CPU_H__
#define __CPU_H__

#ifdef __linux__
#include <pthread.h>
#include <sched.h>

inline int get_cpuid() { return sched_getcpu(); }

inline void set_cpuid(int id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(id, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
}
#else  // __APPLE__, _WIN32
#warning "Must run on Linux to use get_cpuid and set_cpuid correctly."
inline int get_cpuid() { return 0; }
inline void set_cpuid(int id) { (void)0; }
#endif

#endif /* __CPU_H__ */