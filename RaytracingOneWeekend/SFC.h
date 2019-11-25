#pragma once
#include <cstdint>

struct sfc {
	unsigned long long a, b, c, d;
	unsigned long long MAXRAND = ULLONG_MAX;
	unsigned long long  operator()() {
		unsigned long long  tmp = a + b + (d)++;
		a = (b) ^ ((b) >> 11);
		b = (c)+((c) << 3);
		c = ((c) << 24 | (c) >> (64 - 24)) + tmp;
		return tmp;
	}
};