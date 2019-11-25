#pragma once
#include <cstdint>
struct DirkRNG {
	uint64_t s;
	static const uint64_t MAXRAND = (uint64_t)1.84f * 10 ^ 19;
	uint64_t operator()() {
		uint64_t z = (++s ^ 0x9E3779B97F4A7C15ULL) * 0xD1B54A32D192ED03ULL;
		z = (z ^ z >> 28 ^ 0xDB4F0B9175AE2165ULL) * 0xE60E2B722B53AEEBULL;
		return z ^ (z << 19 | z >> 45) ^ (z << 46 | z >> 18);
	}
};