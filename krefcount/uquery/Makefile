all:
	gcc -o fadvise main.c -lpthread
	fallocate -l 1G testfile
	sync

clean:
	rm fadvise
	rm testfile
