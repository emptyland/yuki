#ifndef YUKI_VARINT_H
#define YUKI_VARINT_H

#include "yuki/bits.h"
#include <stdint.h>
#include <stddef.h>

namespace yuki {

class Varint {
public:
	static const size_t kMax64Len = 10;
	static const size_t kMax32Len =  5;

	// For unsigned integer
	static size_t Encode32(uint32_t in, void *out);
	static size_t Encode64(uint64_t in, void *out);

	static inline uint32_t Decode32(const void *in, size_t *len);
	static uint64_t Decode64(const void *in, size_t *len);

    static inline int Sizeof32(uint32_t value);
    static inline int Sizeof64(uint64_t value);

	// For signed integer
	static inline size_t EncodeS64(int64_t in, void *out);
	static inline size_t EncodeS32(int32_t in, void *out);

	static inline int64_t DecodeS64(const void *in, size_t *len);
	static inline int32_t DecodeS32(const void *in, size_t *len);

private:
	Varint() = delete;
	~Varint() = delete;
}; // class Varint

class ZigZag {
public:
	// Signed integer to Unsigned integer
	static inline uint64_t Encode64(int64_t in);
	static inline uint32_t Encode32(int32_t in);

	// Unsigned integer to Signed integer
	static inline int64_t Decode64(uint64_t in);
	static inline int32_t Decode32(uint32_t in);

private:
	ZigZag() = delete;
	~ZigZag() = delete;
}; // class ZigZag

inline uint32_t Varint::Decode32(const void *in, size_t *len) {
	return static_cast<uint32_t>(Decode64(in, len));
}

inline size_t Varint::EncodeS64(int64_t in, void *out) {
	return Encode64(ZigZag::Encode64(in), out);
}

inline size_t Varint::EncodeS32(int32_t in, void *out) {
	return Encode32(ZigZag::Encode32(in), out);
}

inline int64_t Varint::DecodeS64(const void *in, size_t *len) {
	return ZigZag::Decode64(Decode64(in, len));
}

inline int32_t Varint::DecodeS32(const void *in, size_t *len) {
	return ZigZag::Decode32(Decode32(in, len));
}

inline int Varint::Sizeof32(uint32_t value) {
    if (value == 0) {
        return 1;
    } else {
        return (32 - Bits::CountLeadingZeros32(value) + 6) / 7;
    }
}

inline int Varint::Sizeof64(uint64_t value) {
    if (value == 0) {
        return 1;
    } else {
        return (64 - Bits::CountLeadingZeros64(value) + 6) / 7;
    }
}

inline uint64_t ZigZag::Encode64(int64_t in) {
	return in < 0 ? static_cast<uint64_t>(((-in) << 1) | 1ULL) :
		static_cast<uint64_t>(in << 1);
}

inline uint32_t ZigZag::Encode32(int32_t in) {
	return in < 0 ? static_cast<uint32_t>(((-in) << 1) | 1U) :
		static_cast<uint32_t>(in << 1);
}

inline int64_t ZigZag::Decode64(uint64_t in) {
	return in & 0x1ULL ? - static_cast<uint64_t>(in >> 1) :
		static_cast<uint64_t>(in >> 1);
}

inline int32_t ZigZag::Decode32(uint32_t in) {
	return in & 0x1U ? - static_cast<uint32_t>(in >> 1) :
		static_cast<uint32_t>(in >> 1);
}

} // namespace yuki

#endif //YUKI_VARINT_H

