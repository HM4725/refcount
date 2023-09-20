#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <unistd.h>

#define __NR_DISTRIBUTE_REFCOUNT 451
#define __NR_CENTRALIZE_REFCOUNT 452
#define PAYGO_FADV_HOTSECT 8

static inline long distribute_refcount(int fd, loff_t offset, size_t count) {
	return syscall(__NR_DISTRIBUTE_REFCOUNT, fd, offset, count);
}
static inline long centralize_refcount(int fd, loff_t offset, size_t count) {
	return syscall(__NR_CENTRALIZE_REFCOUNT, fd, offset, count);
}

#endif /* __SYSCALLS_H__ */
