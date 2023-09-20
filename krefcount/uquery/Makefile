all:
	gcc -o fadvise fadvise_thread.c -lpthread
	fallocate -l 1G testfile

clean:
	rm fadvise
	rm testfile
