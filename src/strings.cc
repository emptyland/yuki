#include "yuki/strings.h"
#include "yuki/slice.h"
#include "yuki/file_path.h"
#include <memory>

namespace yuki {

std::string Strings::Vformat(const char *fmt, va_list ap) {
	va_list copied;
	int len = kInitialSize, rv = len;
	std::unique_ptr<char[]> buf;
	do {
		len = rv + kInitialSize;
		buf.reset(new char[len]);
		va_copy(copied, ap);
		rv = vsnprintf(buf.get(), len, fmt, ap);
		va_copy(ap, copied);
	} while (rv > len);
	return std::string(buf.get());
}

std::string Strings::Format(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	std::string str(Vformat(fmt, ap));
	va_end(ap);
	return str;
}

Status Strings::FromFile(FileRef name, std::string *str) {
	FILE *fp = fopen(name.c_str(), "rb");
	if (!fp)
		return Status::Systemf("Can not open %s", name.c_str());

	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	rewind(fp);
	str->resize(len + 1);
	len = fread(&str->at(0), 1, len, fp);
	fclose(fp);
	str->resize(len);
	return Status::OK();
}

Status Strings::ToFile(FileRef name, const std::string &buf) {
	FILE *fp = fopen(name.c_str(), "wb");
	if (!fp)
		return Status::Systemf("Can not open %s", name.c_str());

	fwrite(buf.c_str(), 1, buf.size(), fp);
	fclose(fp);
	return Status::OK();
}

std::string Strings::ReadableBinary(SliceRef bin) {
	std::string buf;
	for (size_t i = 0; i < bin.Length(); ++i) {
		uint8_t byte = bin[i];
		if (isprint(bin[i]))
			buf.append(1, bin[i]);
		else
			buf.append(Format("\\x%02x", byte));
	}
	return buf;
}

std::string Strings::SourceBinary(SliceRef bin) {
	std::string buf("{");
	for (size_t i = 0; i < bin.Length(); ++i) {
		uint8_t byte = bin[i];
		if (i) buf.append(", ");
		if (isprint(bin[i]))
			buf.append(Format("\'%c\'", bin[i]));
		else
			buf.append(Format("0x%02x", byte));
	}
	buf.append("};");
	return buf;
}

} // namespace yuki

