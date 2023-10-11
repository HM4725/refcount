# Evaluate query overhead
## 1. Config
### fadvise_thread.c 
```c
#define NPAGES (1024 * 1024 * 1024 / 4)
#define HOTSECT (SIZE * NPAGES)
```
You can adjust options above. 

## 2. Run
```sh
sudo bash run.sh | tee log/{output}.log
```
You can change the output file name.
