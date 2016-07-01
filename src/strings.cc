#include "yuki/strings.h"
#include "yuki/slice.h"
#include "yuki/file_path.h"
#include "pcre.h"
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

/*static*/ Status Strings::Split(const std::string subject,
                                 const std::string &regex,
                                 std::vector<std::string> *part) {
    const char *err = nullptr;
    int err_index = 0;
    auto pcre = pcre_compile(regex.c_str(), PCRE_EXTRA, &err, &err_index,
                             nullptr);
    if (!pcre) {
        return Status::Errorf(Status::kCorruption, "pcre fail: %s", err);
    }

    int result[30] = {0};
    int rv = 0, offset = 0, start = 0;
    while ((rv = pcre_exec(pcre, nullptr, subject.c_str(),
                           static_cast<int>(subject.size()),
                           offset, 0, result, 30)) > 0) {
        offset = result[1];
        part->emplace_back(&subject[start], result[0] - start);
        start = result[1];
    }

    if (subject.size() > static_cast<size_t>(start)) {
        part->emplace_back(&subject[start], subject.size() - start);
    }
    pcre_free(pcre);
    return Status::OK();
}

/*static*/ Status Strings::Split(const char *subject,
                                 const char *regex,
                                 std::vector<Slice> *part) {
    if (!subject) {
        return Status::OK();
    }

    const char *err = nullptr;
    int err_index = 0;
    auto pcre = pcre_compile(regex, PCRE_EXTRA, &err, &err_index,
                             nullptr);
    if (!pcre) {
        return Status::Errorf(Status::kCorruption, "pcre fail: %s", err);
    }

    int subject_len = static_cast<int>(strlen(subject));
    int result[30] = {0};
    int rv = 0, offset = 0, start = 0;
    while ((rv = pcre_exec(pcre, nullptr, subject, subject_len, offset, 0,
                           result, 30)) > 0) {
        offset = result[1];
        part->emplace_back(&subject[start], result[0] - start);
        start = result[1];
    }

    if (subject_len > start) {
        part->emplace_back(&subject[start], subject_len - start);
    }
    pcre_free(pcre);
    return Status::OK();
}

} // namespace yuki

