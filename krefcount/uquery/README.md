# Evaluate query overhead
## 1. Config
### fadvise_thread.c 
```c
#define NTHREADS (32)
#define NPAGES (1024 * 1024 / 4)
#define HOTSECT (SIZE * NPAGES)
```
You can adjust options above. 

## 2. Build
```sh
make
```

## 3. Test
```sh
./fadvise
time echo 1 > /proc/sys/vm/drop_caches
free -h
```
