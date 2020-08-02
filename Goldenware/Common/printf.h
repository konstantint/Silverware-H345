// printf extensions
//
// License: MIT
// Author: konstantint

#pragma once
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include "defines.h"
#include "../RX/common.h"
#include "point3d.h"

void printf(float v);
void printf(const Point3d<float>& point);
void printf(const RxData& rxdata);

inline void printf(const Point3d<int16_t>& point) {
	printf("(%i, %i, %i)", point.x(), point.y(), point.z());
}

#ifdef SERIAL_DEBUG

template<typename T> inline void dprintf(const T& v) {
	printf(v);
}

inline int dprintf(const char* fmt, ...) {
	int result;
	va_list va;
	va_start(va, fmt);
	result = vprintf(fmt, va);
	va_end(va);
	return result;
}

#else

template <typename T> inline void dprintf(const T& v) {}
inline int dprintf(const char* fmt, ...) { return 0; }

#endif

