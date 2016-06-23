#include "yuki/file.h"
#include "yuki/file_path.h"
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <regex>

namespace yuki {

/*static*/
Status File::Remove(FileRef path, bool deep) {
	if (!deep)
		return SYSCALL_IF(remove(path.c_str()) == 0,
				"Can not remove %s", path.c_str());

	// Deep remove, delete all file and directories.
	std::vector<std::string> children;
	bool is_dir = false;
	Status ok = IsDir(path, &is_dir);
	if (ok.Failed())
		return ok;
	if (is_dir) {
		children.clear();
		ok = GetDir(path, "", &children);
		if (ok.Failed())
			return ok;
		for (const auto &child : children) {
			if (child == "." || child == "..")
				continue;
			auto child_path(path);
			child_path.Append(child);
			// Recuresion deletion.
			ok = Remove(child_path, true);
			if (ok.Failed())
				return ok;
		}
	}
	return SYSCALL_IF(remove(path.c_str()) == 0,
			"Can not remove %s", path.c_str());
}

/*static*/
Status File::MakeDir(FileRef path, bool recursion) {
	if (!recursion)
		return SYSCALL_IF(mkdir(path.c_str(), 0755) == 0,
				"Can not make directory %s", path.c_str());

	std::vector<FilePath> partial;
	if (!path.Separate(&partial))
		return Status::Errorf(Status::kInvalidArgument, "Bad directory path %s",
				path.c_str());

	FilePath cur;
	for (const auto &name : partial) {
		cur.Append(name);
		bool is_dir, is_exist;
		Status ok = Exist(cur, &is_exist);
		if (ok.Failed())
			return ok;
		if (!is_exist) {
			ok = MakeDir(cur, false);
			if (ok.Failed())
				return ok;
			continue;
		}
		ok = IsDir(cur, &is_dir);
		if (ok.Failed())
			return ok;
		if (!is_dir)
			return Status::Errorf(Status::kInvalidArgument, "%s not a directory",
					cur.c_str());
	}
	return Status::OK();
}

/*static*/
Status File::IsDir(FileRef path, bool *is_dir) {
	struct stat stub;
	if (stat(path.c_str(), &stub) < 0)
		return Status::Systemf("Can not get %s state", path.c_str());

	*is_dir = ((stub.st_mode & S_IFDIR) != 0);
	return Status::OK();
}

/*static*/
Status File::Exist(FileRef path, bool *is_exist) {
	struct stat stub;
	if (stat(path.c_str(), &stub) == 0) {
		*is_exist = true;
		return Status::OK();
	}
	if (errno != ENOENT)
		return Status::Systemf("Can not get %s state", path.c_str());
	*is_exist = false;
	return Status::OK();
}

/*static*/
Status File::GetDir(FileRef path, const std::string &reg,
		std::vector<std::string> *children) {
	struct dirent entry, *rv;
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return Status::Systemf("Can not open directory %s", path.c_str());

	if (reg.empty()) {
		while (readdir_r(dir, &entry, &rv) == 0 && rv != nullptr) {
			children->emplace_back(rv->d_name);
		}
	} else {
		// Filter by std::regex
		std::regex regex(reg);
		std::cmatch match;
		while (readdir_r(dir, &entry, &rv) == 0 && rv != nullptr) {
			if (std::regex_match(rv->d_name, match, regex)) {
				children->emplace_back(rv->d_name);
			}
		}
	}
	return Status::OK();
}

} // namespace yuki

