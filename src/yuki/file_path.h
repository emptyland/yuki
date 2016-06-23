#ifndef YUKI_FILE_PATH_H
#define YUKI_FILE_PATH_H

#include "yuki/status.h"
#include <string>
#include <vector>

namespace yuki {

class FilePath;
typedef const FilePath &FileRef;

class FilePath {
public:
#if defined(_WIN32)
	// Windows NT use UTF-16 strings
	typedef std::wstring StringType;
#else
	// Posix OS use UTF-8 encoding
	typedef std::string  StringType;
#endif
	typedef StringType::value_type CharType;

	static const char kSeparator[];

	explicit FilePath(const StringType &name) : path_(name) {}
	inline FilePath() = default;
	inline ~FilePath() = default;

	const CharType *c_str() const { return path_.c_str(); }
	const StringType &Get() const { return path_; }
	bool Empty() const { return path_.empty(); }

	//bool operator == (FileRef other) { return path_ == other.path_; }
	//bool operator == (const StringType &other) { return path_ == other; }

	// Unicode getters
	std::string AsUTF8() const;
	std::string AsMB() const;

	// path/to/file.ext -> path/to
	inline FilePath Parent() const;
	// path/to/file.ext -> file
	inline FilePath FileTitle() const;
	// path/to/file.ext -> "file.ext"
	inline FilePath FileName() const;
	// path/to/file.ext -> "ext"
	inline StringType Extension() const;

	// path/to/file.ext    -> ["path", "to", "file.ext"]
	// /path/to/file.ext   -> ["/", "path", "to", "file.ext"]
	// C:\path\to\file.ext -> ["C:\\", "path", "to", "file.ext"]
	bool Separate(std::vector<StringType> *names) const;
	bool Separate(std::vector<FilePath> *names) const;

	Status Exist(bool *is_exist) const;
	bool Absolute() const;

	bool Append(FileRef child);
	bool Append(const StringType &child) { return Append(FilePath{child}); }
	inline bool AppendLiteral(const StringType &child);

private:
	StringType path_;
}; // class FilePath

inline FilePath FilePath::Parent() const {
	auto sep_pos = path_.rfind(kSeparator);
	return FilePath{path_.substr(0, sep_pos)};
}

inline FilePath FilePath::FileName() const {
	auto sep_pos = path_.rfind(kSeparator);
	return FilePath{path_.substr(sep_pos + 1)};
}

inline FilePath FilePath::FileTitle() const {
	auto file_name(FileName());
	auto ext_len = file_name.Extension().size();
	const auto &path = file_name.Get();
	return FilePath{path.substr(0, path.size() - ext_len - 1)};
}

inline FilePath::StringType FilePath::Extension() const {
	size_t i = path_.rfind('.');
	if (i == StringType::npos)
		return StringType();
	return path_.substr(i + 1);
}

inline bool FilePath::AppendLiteral(const FilePath::StringType &str) {
	path_.append(str);
	// TODO: check the child directory string.
	return true;
}

} // namespace yuki

#endif //YUKI_FILE_PATH_H

