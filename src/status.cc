#include "yuki/status.h"
#include "yuki/strings.h"
#include <errno.h>
#include <string.h>

namespace yuki {

/*static*/
Status Status::Verrorf(Status::CodeTy code, const char *fmt, va_list ap) {
	DCHECK(code != kOk);
	int32_t extra = code == kSystemError ? errno : 0;

	std::string msg(Strings::Vformat(fmt, ap));
	uint32_t msg_len = static_cast<uint32_t>(msg.size());

	// [0, 3] Message Length
	// [4] Code
	// [5, 8] Extra Code
	// [9, ...] Message
	char *buf = new char[kMessageOffset + msg_len];
	memcpy(buf, &msg_len, sizeof(msg_len));
	buf[kCodeOffset] = static_cast<char>(code);
	memcpy(buf + kExtraOffset, &extra, sizeof(extra));
	memcpy(buf + kMessageOffset, msg.c_str(), msg_len);

	Status rv;
	rv.state_.reset(buf);
	return rv;
}

std::string Status::ToString() const {
	std::string rv;
	switch (Code()) {
	case kOk:
		return "Ok";

	case kNotFound:
		rv.append("Not found : ");
		break;

	case kCorruption:
		rv.append("Corruption : ");
		break;

	case kNotSupported:
		rv.append("Not Supported : ");
		break;

	case kInvalidArgument:
		rv.append("Invalid Argument : ");
		break;

	case kSystemError:
		rv.append(&state_[kMessageOffset], MessageLength());
		rv.append(" : ");
		rv.append(strerror(ExtraData()));
		return rv;
	}

	rv.append(&state_[kMessageOffset], MessageLength());
	return rv;
}

} // namespace yuki

