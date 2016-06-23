#ifndef YUKI_FILE_H
#define YUKI_FILE_H

#include <functional>
#include <string>
#include <vector>
#include "yuki/status.h"

namespace yuki {

class FilePath;
typedef const FilePath &FileRef;

class File {
public:
	static Status Remove(FileRef, bool deep);
	static Status Move(FileRef, FileRef);
	static Status MakeDir(FileRef, bool recursion);
	static Status IsDir(FileRef, bool *is_dir);
	static Status Exist(FileRef, bool *is_exist);
	static Status GetDir(FileRef, const std::string &reg,
			std::vector<std::string> *);

private:
	File() = delete;
	~File() = delete;
}; // class File

} // namespace yuki

#endif //YUKI_FILE_H

