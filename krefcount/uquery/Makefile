all:
	gcc -o fadvise fadvise_thread.c -lpthread
	fallocate -l 1G testfile
	sync

clean:
	rm fadvise
	rm testfile
