#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "syscalls.h"

#ifndef NTHREADS
	#define NTHREADS (36)
#endif

/* default 10GB */
#define SIZE (4096)
#define NPAGES (1024 * 1024 / 4 * 10)

#define HOTSECT (SIZE * NPAGES)
//#define HOTSECT 0

int fd;

void *worker(void *args) {
	char buf[SIZE];
	long n;
	long id = (long)args;

	setaffinity(id);

	for (n = 0; n < NPAGES; n++) {
		pread(fd, buf, SIZE, n * SIZE);
	}
	return NULL;
}

int main() {
	int ret;
	char buf[SIZE] = {0,};
	pthread_t thid[NTHREADS];
	long n, i;

	if ((fd = open("testfile", O_CREAT | O_RDWR, 644)) == -1) {
		perror("Fail to open");
		exit(1);
	}
	for (n = 0; n < NPAGES; n++) {
		ret = pread(fd, buf, SIZE, n * SIZE); 
	}
	printf("read result: %d\n", ret);
	
#if HOTSECT != 0
	ret = posix_fadvise(fd, 0, HOTSECT, PAYGO_FADV_HOTSECT);
	printf("fadv result: %d\n", ret);
#endif

	for (i = 0; i < NTHREADS; i++) {
		pthread_create(&thid[i], NULL, worker, (void*)i);
	}
	for (i = 0; i < NTHREADS; i++) {
		pthread_join(thid[i], NULL);
	}

	close(fd);
	return 0;
}
