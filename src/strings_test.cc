#include "yuki/strings.h"
#include "yuki/file_path.h"
#include "yuki/file.h"
#include "gmock/gmock.h"
#include <memory>

namespace yuki {

TEST(StringsTest, Format) {
	std::string str(std::move(Strings::Format("%d", 100)));
	ASSERT_EQ("100", str);

	static const int kLen = 1024;
	std::unique_ptr<char[]> buf{new char[kLen]};
	memset(buf.get(), 'a', kLen);
	str = std::move(Strings::Format("%.*s", kLen, buf.get()));
	std::string expected(kLen, 'a');
	ASSERT_EQ(expected, str);
}

TEST(StringsTest, File) {
	const FilePath kFile{__FILE__ ".tmp"};
	static const char *kText = "Hello, World!\nGood good study!\nDay Day up\n";
	std::string buf{kText};

	ASSERT_TRUE(!!Strings::ToFile(kFile, buf));
	std::string from;
	ASSERT_TRUE(!!Strings::FromFile(kFile, &from));
	ASSERT_EQ(buf, from);
	ASSERT_TRUE(!!File::Remove(kFile, false));
}

} // namespace yuki

