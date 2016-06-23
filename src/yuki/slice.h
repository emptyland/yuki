#ifndef YUKI_SLICE_H
#define YUKI_SLICE_H

#include "yuki/utils.h"
#include "glog/logging.h"
#include <string.h>
#include <stdint.h>
#include <string>

namespace yuki {

class Slice;
typedef const Slice &SliceRef;

class Slice {
public:
	inline Slice() = default;
	inline Slice(const Slice &) = default;
	inline ~Slice() = default;

	explicit Slice(const char *buf) : Slice(buf, strlen(buf)) {}
	inline explicit Slice(const std::string &str);
	inline Slice(const char *buf, size_t len);

	bool Empty() const { return len_ == 0; }
	const char *Data() const { return buf_; }
	const uint8_t *Bytes() const { return bit_cast<const uint8_t*>(buf_); }
	size_t Length() const { return len_; }

	std::string ToString() const {return std::string(Data(), Length());}

	void Clear() { buf_ = ""; len_ = 0; }

	inline char operator[] (size_t index) const;
	inline size_t StartWith(SliceRef other) const;

	inline int Compare(SliceRef other) const;

private:
	const char *buf_ = "";
	size_t len_ = 0;
}; // class Slice

inline Slice::Slice(const std::string &str)
	: Slice(str.c_str(), str.size()) {
}

inline Slice::Slice(const char *buf, size_t len)
	: buf_(DCHECK_NOTNULL(buf))
	, len_(len) {
}

inline size_t Slice::StartWith(SliceRef other) const {
	size_t len = std::min(other.Length(), Length()), start = 0;
	for (size_t i = 0; i < len; ++i) {
		if (buf_[i] == other[i])
			++start;
		else
			break;
	}
	return start;
}

inline char Slice::operator[] (size_t index) const {
	DCHECK_LT(index, Length());
	return Data()[index];
}

inline int Slice::Compare(SliceRef other) const {
	if (this == &other)
		return 0;
	auto min_len = std::min(Length(), other.Length());
	auto rv = memcmp(Bytes(), other.Bytes(), min_len);
	if (rv == 0) {
		if (Length() < other.Length())
			rv = -1;
		else if (Length() > other.Length())
			rv = +1;
	}
	return rv;
}

} // namespace yuki

#endif //YUKI_SLICE_H

