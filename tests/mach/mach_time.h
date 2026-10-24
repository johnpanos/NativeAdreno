/* SPDX-License-Identifier: MIT */
/* Host double of <mach/mach_time.h> for the kernel sources built against the test fixtures (tests/ on the include path): Apple hosts
 * take the system header; elsewhere mach_absolute_time is the monotonic clock in nanoseconds (timebase 1/1). Authored, not XNU's. */
#ifndef NATIVE_ADRENO_TEST_MACH_TIME_H
#define NATIVE_ADRENO_TEST_MACH_TIME_H
#if defined(__APPLE__)
#include_next <mach/mach_time.h>
#else
#include <cstdint>
#include <ctime>
inline uint64_t mach_absolute_time() {
    timespec t{};clock_gettime(CLOCK_MONOTONIC,&t);
    return static_cast<uint64_t>(t.tv_sec)*1000000000ull+static_cast<uint64_t>(t.tv_nsec);
}
#endif
#endif
