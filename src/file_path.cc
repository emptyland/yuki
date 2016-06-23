#include "yuki/file_path.h"
#include "yuki/file.h"
#include "glog/logging.h"
#include <regex>

namespace yuki {

#if defined(_WIN32)
const char FilePath::kSeparator[] = "\\";
#else
const char FilePath::kSeparator[] = "/";
#endif

std::string FilePath::AsMB() const {
#if defined(_WIN32)
	// TODO:
#else
	return path_;
#endif
}

std::string FilePath::AsUTF8() const {
#if defined(_WIN32)
	// TODO:
#else
	return path_;
#endif
}

Status FilePath::Exist(bool *is_exist) const {
	return File::Exist(*this, is_exist);
}

bool FilePath::Absolute() const {
	if (path_.empty())
		return false;
#if defined(_WIN32)
	// TODO:
#else
	return path_.find(kSeparator) == 0U;
#endif
}

bool FilePath::Append(FileRef child) {
	if (child.Empty() || child.Absolute())
		return false;
	if (!Empty())
		path_.append(kSeparator);
	path_.append(child.Get());
	return true;
}

bool FilePath::Separate(std::vector<FilePath::StringType> *names) const {
	size_t i, p = path_.size();
	while ((i = path_.rfind(kSeparator, p)) != StringType::npos && i != 0) {
		names->emplace(names->begin(), path_.substr(i + 1, p - i));
		p = i - 1;
	}
	names->emplace(names->begin(), path_.substr(i + 1, p - i));
	if (i == 0)
		names->emplace(names->begin(), "/");
	return names->size() > 0;
}

bool FilePath::Separate(std::vector<FilePath> *names) const {
	std::vector<StringType> partial;
	if (!Separate(&partial))
		return false;
	for (auto name : partial)
		names->emplace_back(name);
	return true;
}

} // namespace yuki

