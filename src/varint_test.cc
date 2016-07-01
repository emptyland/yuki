#include "yuki/varint.h"
#include "gmock/gmock.h"
#include <random>

namespace yuki {

TEST(VarintTest, Sanity) {
	uint8_t buf[Varint::kMax64Len];

	ASSERT_EQ(1U, Varint::Encode64(0, buf));
	ASSERT_EQ(0x0, buf[0]);

	ASSERT_EQ(1U, Varint::Encode64(1, buf));
	ASSERT_EQ(0x1, buf[0]);

	ASSERT_EQ(1U, Varint::Encode32(0, buf));
	ASSERT_EQ(0x0, buf[0]);

	ASSERT_EQ(1U, Varint::Encode32(1, buf));
	ASSERT_EQ(0x1, buf[0]);
}

TEST(VarintTest, ZigZag) {
	int64_t i64[] = { 0, 1, 2, 100, 0xffff, 0x7fffffff, 0x7fffffffffffffffLL};
	for (auto i : i64) {
		uint64_t p = ZigZag::Encode64(i);
		uint64_t n = ZigZag::Encode64(-i);
		ASSERT_EQ(static_cast<uint64_t>(i << 1), p);
		ASSERT_EQ(i, ZigZag::Decode64(p));
		if (i) {
			ASSERT_EQ(static_cast<uint64_t>(i << 1) | 1U, n);
			ASSERT_EQ(-i, ZigZag::Decode64(n));
		}
	}

	int32_t i32[] = { 0, 1, 2, 100, 65535, 0x7fffffff };
	for (auto i : i32) {
		uint32_t p = ZigZag::Encode32(i);
		uint32_t n = ZigZag::Encode32(-i);
		ASSERT_EQ(static_cast<uint32_t>(i << 1), p);
		ASSERT_EQ(i, ZigZag::Decode32(p));
		if (i) {
			ASSERT_EQ(static_cast<uint32_t>(i << 1) | 1U, n);
			ASSERT_EQ(-i, ZigZag::Decode32(n));
		}
	}
}

TEST(VarintTest, DISABLED_Encoding) {
	uint64_t in = 15028999435905310454ULL;
	uint8_t buf[Varint::kMax64Len];
	size_t len;
	len = Varint::Encode64(in, buf);
	ASSERT_GE(len, 1U);
	ASSERT_EQ(in, Varint::Decode64(buf, &len));
}

TEST(VarintTest, UnsignedFuzzy) {
	std::uniform_int_distribution<uint64_t> dist(0ULL,
			0xffffffffffffffffULL);
	std::mt19937 engine;
	auto gen = std::bind(dist, engine);
	uint8_t buf[Varint::kMax64Len];
	for (auto i = 0; i < 1000000; ++i) {
		auto in = gen();
		size_t ol = 0, il = Varint::Encode64(in, buf);
		ASSERT_GE(il, 1U);
		ASSERT_EQ(in, Varint::Decode64(buf, &ol));
		ASSERT_EQ(il, ol);
	}
}

TEST(VarintTest, SignedFuzzy) {
	std::uniform_int_distribution<int64_t> dist(
			-0x7fffffffffffffffULL,
			0x7fffffffffffffffULL);
	std::mt19937 engine;
	auto gen = std::bind(dist, engine);
	uint8_t buf[Varint::kMax64Len];
	for (auto i = 0; i < 1000000; ++i) {
		auto in = gen();
		size_t ol = 0, il = Varint::EncodeS64(in, buf);
		ASSERT_GE(il, 1U);
		ASSERT_EQ(in, Varint::DecodeS64(buf, &ol));
		ASSERT_EQ(il, ol);
	}
}

TEST(VarintTest, Sizeof32) {
    ASSERT_EQ(1, Varint::Sizeof32(0));
    ASSERT_EQ(2, Varint::Sizeof32(1 << 7));
    ASSERT_EQ(2, Varint::Sizeof32(1 << 8));
    ASSERT_EQ(2, Varint::Sizeof32(1 << 13));

    ASSERT_EQ(3, Varint::Sizeof32(1 << 14));
    ASSERT_EQ(3, Varint::Sizeof32(1 << 20));

    ASSERT_EQ(4, Varint::Sizeof32(1 << 21));
    ASSERT_EQ(4, Varint::Sizeof32(1 << 27));

    ASSERT_EQ(5, Varint::Sizeof32(1 << 28));
    ASSERT_EQ(5, Varint::Sizeof32(1 << 31));
}

TEST(VarintTest, Sizeof64) {
    ASSERT_EQ(1, Varint::Sizeof64(0));
    ASSERT_EQ(2, Varint::Sizeof64(1 << 7));
    ASSERT_EQ(2, Varint::Sizeof64(1 << 8));
    ASSERT_EQ(2, Varint::Sizeof64(1 << 13));

    ASSERT_EQ(3, Varint::Sizeof64(1 << 14));
    ASSERT_EQ(3, Varint::Sizeof64(1 << 20));

    ASSERT_EQ(4, Varint::Sizeof64(1 << 21));
    ASSERT_EQ(4, Varint::Sizeof64(1 << 27));

    ASSERT_EQ(5, Varint::Sizeof64(1 << 28));
    ASSERT_EQ(5, Varint::Sizeof64(1UL << 34));

    ASSERT_EQ(6, Varint::Sizeof64(1UL << 35));
    ASSERT_EQ(6, Varint::Sizeof64(1UL << 41));

    ASSERT_EQ(7, Varint::Sizeof64(1UL << 42));
    ASSERT_EQ(7, Varint::Sizeof64(1UL << 48));

    ASSERT_EQ(8, Varint::Sizeof64(1UL << 49));
    ASSERT_EQ(8, Varint::Sizeof64(1UL << 55));

    ASSERT_EQ(9, Varint::Sizeof64(1UL << 56));
    ASSERT_EQ(9, Varint::Sizeof64(1UL << 62));

    ASSERT_EQ(10, Varint::Sizeof64(1UL << 63));
}

} // namespace yuki

