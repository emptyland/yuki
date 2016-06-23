#include "yuki/skiplist.h"
#include "gmock/gmock.h"
#include <stdint.h>
#include <thread>

namespace yuki {

inline uint32_t MakeKey(uint32_t k, uint32_t v) {
	return (k << 16) | v;
}

inline uint32_t GetKey(uint32_t key) {
	return (key & 0xffff0000) >> 16;
}

inline uint32_t GetValue(uint32_t key) {
	return (key & 0xffff);
}

struct KeyPred {
	int operator () (uint32_t lhs, uint32_t rhs) const {
		lhs = GetKey(lhs); rhs = GetKey(rhs);
		if (lhs < rhs)
			return -1;
		else if (lhs > rhs)
			return 1;
		return 0;
	}
};

TEST(SkipListTest, Sanity) {
	Zone zone(true);
	SkipList<uint32_t, KeyPred> list(KeyPred(), &zone);

	list.Insert(MakeKey(1, 100));
	list.Insert(MakeKey(3, 300));
	list.Insert(MakeKey(5, 500));
	list.Insert(MakeKey(7, 700));
	list.Insert(MakeKey(11, 1100));

	SkipList<uint32_t, KeyPred>::Iterator iter(&list);
	ASSERT_TRUE(iter.Seek(MakeKey(1, 0)));
	ASSERT_EQ(100U, GetValue(iter.Key()));

	ASSERT_TRUE(iter.Seek(MakeKey(3, 0)));
	ASSERT_EQ(300U, GetValue(iter.Key()));

	ASSERT_TRUE(iter.Seek(MakeKey(7, 0)));
	ASSERT_EQ(700U, GetValue(iter.Key()));
}

TEST(SkipListTest, Concurrent) {
	Zone zone(true);
	SkipList<uint32_t, KeyPred> list(KeyPred(), &zone);

	std::thread reader([&list] () {
		SkipList<uint32_t, KeyPred>::Iterator iter(&list);
		while (iter.Next())
			ASSERT_EQ(GetKey(iter.Key()), GetValue(iter.Key()));
	});
	for (int i = 0; i < 500; ++i)
		list.Insert(MakeKey(i, i));
	reader.join();
}

} // namespace yuki

