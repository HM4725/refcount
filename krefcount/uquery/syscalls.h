#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <unistd.h>
#include <sched.h>

#define PAYGO_FADV_HOTSECT 8

static inline int setaffinity(int c)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(c, &cpuset);
	return sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

#endif /* __SYSCALLS_H__ */
