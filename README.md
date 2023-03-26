# Scalable Reference Count
This is the graduation project with [DBOS Lab, HYU](http://dbos.hanyang.ac.kr/).

## Introduction
Linux kernel uses `struct page` to manage physical pages [1]. It has `_refcount` field. On referring a page, the count increases by 1. Then after finishing the reference, the count decreases by 1. This referece counting helps pages not to reclaim on unintential points.

But multiple readers can read the count and multiple writers can write the count. So there is the posibility of contention. DRBH benchmark of fxmark shows the bottleneck [2].

## Previous works
1. Traditional
2. Cotention Distribution
3. Cache Affinity
4. Per-core Hash
5. PAYGO
6. Lodic

## Rationale
PAYGO is not completely free from the counting-query tradeoff.

## Goal
1. Implement PAYGO in user-level
2. Implement PAYGO in kernel-level (linux-v6.2.0)
2. Improve PAYGO

## References
[1] [Kernel source linux/include/linux/mm_types.h](https://elixir.bootlin.com/linux/v6.2/source/include/linux/mm_types.h#L35)

[2] [Understanding Manycore Scalability of File Systems](https://taesoo.kim/pubs/2016/min:fxmark.pdf)