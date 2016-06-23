#include "yuki/slice.h"
#include "gmock/gmock.h"

namespace yuki {

TEST(SliceTest, Sanity) {
	Slice slice;
	ASSERT_TRUE(slice.Empty());
	ASSERT_STREQ("", slice.Data());
}

TEST(SliceTest, ToString) {
	Slice slice{"abc\0def", 7};
	std::string str("abc\0def", 7);
	ASSERT_EQ(str, slice.ToString());

	Slice slice2(str);
	ASSERT_EQ(str, slice2.ToString());
}

TEST(SliceTest, StartWith) {
	Slice slice("a");
	ASSERT_EQ(1U, slice.StartWith(Slice("ab")));
	ASSERT_EQ(0U, slice.StartWith(Slice("b")));

	slice = Slice("ab");
	ASSERT_EQ(2U, slice.StartWith(Slice("abc")));

	slice = Slice("abcd");
	ASSERT_EQ(2U, slice.StartWith(Slice("abde")));
}

} // namespace yuki

