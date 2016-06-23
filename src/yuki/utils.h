#ifndef YUKI_UTILS_H
#define YUKI_UTILS_H

#include <stddef.h>
#include "glog/logging.h"

namespace yuki {

struct Linker {
	enum Type { INITIALIZER, };
};

template <class T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#ifndef _MSC_VER
template <class T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];
#endif

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

template<class T, class F>
inline T bit_cast(F from) {
	static_assert(sizeof(T) == sizeof(F), "Size must be equals.");
	union {
		F in;
		T out;
	};
	in = from; return out;
}

template<class T, class F>
inline T *down_cast(F *from) {
#ifndef NDEBUG
	DCHECK_NOTNULL(dynamic_cast<T*>(from));
#endif
	return static_cast<T*>(from);
}

#define IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)

// Returns true iff x is a power of 2 (or zero). Cannot be used with the
// maximally negative value of the type T (the -1 overflows).
template <typename T>
inline bool IsPowerOf2(T x) {
  return IS_POWER_OF_TWO(x);
}

// Compute the 0-relative offset of some absolute value x of type T.
// This allows conversion of Addresses and integral types into
// 0-relative int offsets.
template <typename T>
inline intptr_t OffsetFrom(T x) {
  return x - static_cast<T>(0);
}


// Compute the absolute value of type T for some 0-relative offset x.
// This allows conversion of 0-relative int offsets into Addresses and
// integral types.
template <typename T>
inline T AddressFrom(intptr_t x) {
  return static_cast<T>(static_cast<T>(0) + x);
}


// Return the largest multiple of m which is <= x.
template <typename T>
inline T RoundDown(T x, intptr_t m) {
  DCHECK(IsPowerOf2(m));
  return AddressFrom<T>(OffsetFrom(x) & -m);
}


// Return the smallest multiple of m which is >= x.
template <typename T>
inline T RoundUp(T x, intptr_t m) {
  return RoundDown<T>(static_cast<T>(x + m - 1), m);
}

} // namespace yuki

#endif //YUKI_UTILS_H

