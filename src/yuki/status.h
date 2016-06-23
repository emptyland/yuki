#ifndef YUKI_STATUS_H
#define YUKI_STATUS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <memory>
#include <string>
#include "glog/logging.h"

namespace yuki {

class Status {
public:
	enum CodeTy {
		kOk = 0,
		kNotFound = 1,
		kCorruption = 2,
		kNotSupported = 3,
		kInvalidArgument = 4,
		kSystemError = 5,
	};

	Status() = default;

	Status(Status &&other) { state_.reset(other.state_.release()); }

	~Status() = default;

	void operator = (Status &&other) { state_.reset(other.state_.release()); }
	explicit operator bool() const { return Ok(); }

	static Status OK() { return Status(); }

	static Status Verrorf(CodeTy code, const char *fmt, va_list ap);
	inline static Status Errorf(CodeTy code, const char *fmt, ...);
	inline static Status Systemf(const char *fmt, ...);

	inline CodeTy Code() const;

	std::string ToString() const;

	bool Ok() const { return Code() == kOk; }
	bool Failed() const { return !Ok(); }

private:
	static const int kLengthOffset = 0;
	static const int kCodeOffset = 4;
	static const int kExtraOffset = 5;
	static const int kMessageOffset = 9;

	Status(const Status &) = delete;
	void operator = (const Status &) = delete;

	uint32_t MessageLength() const {
		DCHECK(Failed());
		return *reinterpret_cast<uint32_t*>(state_.get());
	}

	int32_t ExtraData() const {
		DCHECK(Failed());
		return *reinterpret_cast<int32_t*>(&state_[kExtraOffset]);
	}

	std::unique_ptr<char[]> state_;
}; // class Status

inline Status::CodeTy Status::Code() const {
	return !state_ ? kOk : static_cast<CodeTy>(state_[kCodeOffset]);
}

/*static*/
inline Status Status::Errorf(Status::CodeTy code, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	Status rv(Verrorf(code, fmt, ap));
	va_end(ap);
	return rv;
}

inline Status Status::Systemf(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	Status rv(Verrorf(kSystemError, fmt, ap));
	va_end(ap);
	return rv;
}

#define SYSCALL_IF(cond, ...) \
	((cond) ? Status::OK() : Status::Systemf(__VA_ARGS__))

} // namespace yuki

#endif //YUKI_STATUS_H

