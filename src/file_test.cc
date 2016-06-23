#include "yuki/file.h"
#include "yuki/file_path.h"
#include "gmock/gmock.h"

namespace yuki {

TEST(FileTest, Sanity) {
	bool check = false;
	FilePath path{"file_testing"};

	ASSERT_TRUE(!!File::Exist(path, &check));
	ASSERT_FALSE(check);

	ASSERT_TRUE(!!File::MakeDir(path, false));
	ASSERT_TRUE(!!File::Exist(path, &check));
	ASSERT_TRUE(check);

	ASSERT_TRUE(!!File::Remove(path, false));
}

TEST(FileTest, Mkdir) {
	bool check = false;
	FilePath path{"file_testing/path/to"};

	ASSERT_TRUE(!!File::MakeDir(path, true));
	ASSERT_TRUE(!!File::Exist(path, &check));
	ASSERT_TRUE(check);
	ASSERT_TRUE(!!File::IsDir(path, &check));
	ASSERT_TRUE(check);

	ASSERT_TRUE(!!File::Remove(FilePath{"file_testing"}, true));
}

TEST(FileTest, FilePathSeparate) {
	std::vector<FilePath::StringType> partial;
	FilePath abs_path{"/path/to/file.exe"};
	abs_path.Separate(&partial);
	ASSERT_EQ("/",        partial[0]);
	ASSERT_EQ("path",     partial[1]);
	ASSERT_EQ("to",       partial[2]);
	ASSERT_EQ("file.exe", partial[3]);
	partial.clear();

	FilePath non_path{"path/to/file.exe"};
	non_path.Separate(&partial);
	ASSERT_EQ("path",     partial[0]);
	ASSERT_EQ("to",       partial[1]);
	ASSERT_EQ("file.exe", partial[2]);
}

TEST(FileTest, FilePathPartial) {
	FilePath abs_path{"/path/to/file.exe"};
	EXPECT_EQ("/path/to",  abs_path.Parent().Get());
	EXPECT_EQ("exe",       abs_path.Extension());
	EXPECT_EQ("file.exe",  abs_path.FileName().Get());
	EXPECT_EQ("file",      abs_path.FileTitle().Get());

	FilePath full_path{"path/to/file.exe"};
	EXPECT_EQ("path/to",  full_path.Parent().Get());
	EXPECT_EQ("exe",      full_path.Extension());
	EXPECT_EQ("file.exe", full_path.FileName().Get());
	EXPECT_EQ("file",     full_path.FileTitle().Get());
}

} // namespace yuki

