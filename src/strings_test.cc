#include "yuki/strings.h"
#include "yuki/file_path.h"
#include "yuki/file.h"
#include "gmock/gmock.h"
#include "pcre.h"
#include <memory>

namespace yuki {

TEST(StringsTest, Format) {
	std::string str(Strings::Format("%d", 100));
	ASSERT_EQ("100", str);

	static const int kLen = 1024;
	std::unique_ptr<char[]> buf{new char[kLen]};
	memset(buf.get(), 'a', kLen);
	str = Strings::Format("%.*s", kLen, buf.get());
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

TEST(StringsTest, Splitting) {
    std::vector<std::string> part;

    auto rs = Strings::Split("a b     c", "\\s+", &part);
    ASSERT_TRUE(rs.Ok());

    ASSERT_EQ(3U, part.size());
    ASSERT_EQ("a", part[0]);
    ASSERT_EQ("b", part[1]);
    ASSERT_EQ("c", part[2]);

    part.clear();
    rs = Strings::Split("a", "\\s+", &part);
    ASSERT_TRUE(rs.Ok());

    ASSERT_EQ(1U, part.size());
    ASSERT_EQ("a", part[0]);
}

} // namespace yuki

