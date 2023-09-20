#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "syscalls.h"

#define NTHREADS (32)
#define SIZE (4096)
#define NPAGES (1024 * 1024 / 4)
//#define HOTSECT (SIZE * NPAGES / 100)
#define HOTSECT 0

int fd;

void *worker(void *args) {
	char buf[SIZE];
	int i;

	for (i = 0; i < NPAGES; i++) {
		pread(fd, buf, SIZE, i * SIZE);
	}
	return NULL;
}

int main() {
	int ret;
	char *buf;
	pthread_t thid[NTHREADS];
	int i;

	/* */
	buf = malloc(SIZE * NPAGES);
	if ((fd = open("testfile", O_CREAT | O_RDWR, 644)) == -1) {
		perror("Fail to open");
		exit(1);
	}
	ret = read(fd, buf, SIZE * NPAGES); 
	printf("read result: %d\n", ret);
	
#if HOTSECT != 0
	ret = posix_fadvise(fd, 0, HOTSECT, PAYGO_FADV_HOTSECT);
	printf("fadv result: %d\n", ret);
#endif

	for (i = 0; i < NTHREADS; i++) {
		pthread_create(&thid[i], NULL, worker, NULL);
	}
	for (i = 0; i < NTHREADS; i++) {
		pthread_join(thid[i], NULL);
	}

	free(buf);

	close(fd);
	return 0;
}
