# User reference count

## Build
```sh
cmake -B build .
cd build
make -j
```
## Run benchmark
```sh
./bin/urefcount_benchmark
```
## Log file
`log.csv`
### Example
```
N,iters,type
1,12574066,virtual_file
1,14344544,virtual_file_noop
1,14227208,virtual_file_nonatomic
1,13169667,virtual_file_cache_affinity
1,10134581,virtual_file_refcache
2,18166104,virtual_file
2,27589004,virtual_file_noop
2,25125565,virtual_file_nonatomic
2,25610319,virtual_file_cache_affinity
2,19756109,virtual_file_refcache
...
```