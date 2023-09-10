#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "syscalls.h"

int main() {
	int fd;
	long ret;

	if ((fd = open("testfile.txt", O_CREAT | O_RDWR, 644)) == -1) {
		perror("Fail to open");
		exit(1);
	}
	ret = distribute_refcount(fd, 0, 4096);
	printf("distribute_refcount: %ld\n", ret);
	ret = centralize_refcount(fd, 0, 4096);
	printf("centralize_refcount: %ld\n", ret);

	close(fd);
	return 0;
}
