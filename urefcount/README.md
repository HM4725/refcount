# User reference count
![Static Badge](https://img.shields.io/badge/cmake-3.22-008FBA)

## 1) Getting Started
### Build (benchmark)
```sh
cmake -B build .
cmake --build ./build
```
### Installation (plotter)
```sh
pip install -r requirements.txt
```

## 2) Benchmark
### Run
```sh
./build/bin/urefcount_benchmark
```
### Result file
`log.csv`
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
### Plot
```sh
python3 plotter.py --log=log.csv --output=result.png
```

## 3) Evaluation
### Our environment
```
- 24 cores (12 cores/CPU, 2 sockets)
    (Intel(R) Xeon(R) CPU E5-2699 v3 @ 2.30GHz)
- 128GB DRAM
- Ubuntu 22.04.3 LTS (5.15.0-84-generic)
```

### Result: result.png
![result-24cores](./img/result-24cores.jpeg)