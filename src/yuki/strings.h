#ifndef YUKI_STRINGS_H
#define YUKI_STRINGS_H

#include "yuki/status.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>

namespace yuki {

class Slice;
typedef const Slice &SliceRef;

class FilePath;
typedef const FilePath &FileRef;

class Strings {
public:
	static const int kInitialSize = 64;

	// String formating:
	static std::string Vformat(const char *fmt, va_list ap);

	__attribute__ (( __format__ (__printf__, 1, 2)))
	static std::string Format(const char *fmt, ...);

    /**
     * split string "total" by "regex" to part
     */
    static Status Split(const std::string subject,
                        const std::string &regex,
                        std::vector<std::string> *part);

    static Status Split(const char *subject,
                        const char *regex,
                        std::vector<Slice> *part);

	// Read or write a string in a file:
	static Status FromFile(FileRef name, std::string *buf);
	static Status ToFile(FileRef name, const std::string &buf);

	// Binary blob display:
	static std::string ReadableBinary(SliceRef bin);
	static std::string SourceBinary(SliceRef bin);

private:
	Strings() = delete;
	Strings(const Strings &) = delete;
	void operator = (const Strings &) = delete;
}; // class Strings

} // namespace yuki

#endif //YUKI_STRINGS_H

