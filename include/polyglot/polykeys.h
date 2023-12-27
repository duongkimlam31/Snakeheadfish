#ifndef POLYKEYS_H
#define POLYKEYS_H
#include <stdint.h>


#ifdef _MSC_VER
#  define U64(u) (u##ui64)
#else
#  define U64(u) (u##ULL)
#endif

extern const uint64_t Random64[781];


#endif