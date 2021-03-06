# Specification

## Abstract

Fixed-Length Integer Trim (FLIT) is a byte oriented
[universal code](https://en.wikipedia.org/wiki/Universal_code_%28data_compression%29).
FLIT64 outperforms VLQ, a.k.a. varint, in terms of speed and size.

* Fixed serial size range by design
* Size known at fist byte
* CPU friendly: count zeros & bit shift, no loops

This is free and unencumbered software released into the
[public domain](http://creativecommons.org/publicdomain/zero/1.0).


## FLIT64

The 64-bit unsigned version encodes an integer in 1 to 9 octets.

The first octet advertises the number of octets following with the trailing
zero count. Any remaining bits hold the least significant data bits and the
following octets, if any at all, hold the rest in little-endian order.

| Total Size | First Bits  | Range                               |
|:-----------|:------------|:------------------------------------|
| 1 octet    | `xxxx xxx1` | 7-bit (128)                         |
| 2 octets   | `xxxx xx10` | 14-bit (16'384)                     |
| 3 octets   | `xxxx x100` | 21-bit (2'097'152)                  |
| 4 octets   | `xxxx 1000` | 28-bit (268'435'456)                |
| 5 octets   | `xxx1 0000` | 35-bit (34'359'738'368)             |
| 6 octets   | `xx10 0000` | 42-bit (4'398'046'511'104)          |
| 7 octets   | `x100 0000` | 49-bit (562'949'953'421'312)        |
| 8 octets   | `1000 0000` | 56-bit (72'057'594'037'927'936)     |
| 9 octets   | `0000 0000` | 64-bit (18'446'744'073'709'551'616) |

Encoding *should* pick the smallest range capable to hold the value.


### Decoding Example

1. The first byte read is `1010 0110`.
2. One tailing zero means we need to read one more byte, which is `0000 1111`.
3. The two bytes in little-endian order make `0000 1111 1010 0110`.
4. Drop the two size bits with a bit shift to get `0000 0011 1110 1001` or decimal 1001.


## FLIT64S

For signed integers, the algorithm is preceded with *ZigZag encoding*. See the
[Protocol Buffers Encoding](https://developers.google.com/protocol-buffers/docs/encoding#signed-integers)
documentation for detailed information.

## Benchmark

Encoding and decoding in C is almost at memcpy speed on a Mac Pro (Late 2013).

```
------------------------------------------------------
Benchmark               Time           CPU Iterations
------------------------------------------------------
BM_flit64_enc/1          2 ns          2 ns  418162594   4.52895GB/s   579.706M items/s
BM_flit64_enc/5          2 ns          2 ns  372356270   3.97295GB/s   508.538M items/s
BM_flit64_enc/9          2 ns          2 ns  444419049   4.80462GB/s   614.991M items/s
BM_flit64_dec/1          2 ns          2 ns  353722934   3.78997GB/s   485.117M items/s
BM_flit64_dec/5          2 ns          2 ns  354477045   3.79497GB/s   485.756M items/s
BM_flit64_dec/9          2 ns          2 ns  450094198   4.80574GB/s   615.134M items/s
BM_memcpy64             2 ns          2 ns  413846108   4.50834GB/s   577.068M items/s
```

The implementation in Go
[![(GoDoc)](https://godoc.org/github.com/pascaldekloe/flit?status.svg)](https://godoc.org/github.com/pascaldekloe/flit)
needs a bit more time due to limited inlining. The "Raw" benchmarks are as fast
as its gets with fixed-width encoding.


```
BenchmarkPutUint64-12       	200000000	         6.13 ns/op	1304.65 MB/s
BenchmarkPutUint64Raw-12    	2000000000	         1.96 ns/op	4085.82 MB/s
BenchmarkPutUint64VQL-12    	200000000	         7.22 ns/op	1108.19 MB/s
BenchmarkUint64-12          	200000000	         7.03 ns/op	1138.26 MB/s
BenchmarkUint64Raw-12       	1000000000	         2.08 ns/op	3842.63 MB/s
BenchmarkUint64VQL-12       	100000000	        11.0 ns/op	 730.57 MB/s
```



## Credits

* Aleksey Demakov @ademakov
* Roman Gershman @romange
