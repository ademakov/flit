#include "flit.h"

#include <benchmark/benchmark.h>

#include <stdint.h>
#include <string.h>


uint8_t* buf = new uint8_t[9];
uint64_t v64;


static inline void setup(benchmark::State& state) {
	v64 = (uint64_t)1 << ((state.range(0) - 1) * 7);
	size_t n = flit64_enc(buf, v64);
	if (n != state.range(0)) {
		state.SkipWithError("size miss");
	}
}

static inline void roundup(benchmark::State& state, size_t byteCount) {
	state.SetItemsProcessed(state.iterations());
	state.SetBytesProcessed(state.iterations() * byteCount);
}

static void BM_flit64_enc(benchmark::State& state) {
	setup(state);

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(flit64_enc(buf, v64));
		benchmark::ClobberMemory();
	}

	roundup(state, 8);
}

static void BM_flit64_dec(benchmark::State& state) {
	setup(state);

	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(flit64_dec(&v64, buf));
	}

	roundup(state, 8);
}

static void BM_memcpy64(benchmark::State& state) {
	v64 = 123456789;

	while (state.KeepRunning()) {
		memcpy(buf, &v64, 8);
		benchmark::ClobberMemory();
	}

	roundup(state, 8);
}

BENCHMARK(BM_flit64_enc)->DenseRange(1, 9, 4);
BENCHMARK(BM_flit64_dec)->DenseRange(1, 9, 4);
BENCHMARK(BM_memcpy64);

BENCHMARK_MAIN();
