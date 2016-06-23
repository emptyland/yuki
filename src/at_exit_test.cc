#include "yuki/at_exit.h"
#include "gmock/gmock.h"

namespace yuki {

TEST(AtExit, Sanity) {
	int flag = 0;
	{
		AtExit scoped_exit{Linker::INITIALIZER};
		AtExit::Current()->AddHook([&flag] () {
			++flag;
		});
		ASSERT_EQ(0, flag);
	}
	ASSERT_EQ(1, flag);
}

} // namespace yuki

