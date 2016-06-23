#include "yuki/zone.h"
#include "yuki/utils.h"
#include "gmock/gmock.h"

namespace yuki {
namespace {

template<size_t N>
class Stub : public ZoneObject {
public:
	static const char kFilled = 0xaa;
	Stub() {
		for (size_t i = 0; i < arraysize(payload_); ++i)
			payload_[i] = kFilled;
	}

	virtual ~Stub() = default;

	bool Verify() const {
		for (auto c : payload_)
			if (c != kFilled)
				return false;
		return true;
	}

private:
	char payload_[N];
};

class ScopeStub : public ZoneObject {
public:
	ScopeStub(int *p) : i_(p) { *i_ = 0; }
	virtual ~ScopeStub() { *i_ = 1; }

private:
	int *i_;
};

} // namespace

TEST(ZoneTest, Sanity) {
	Zone zone;
	size_t max = Zone::kPageSize /
		RoundUp(sizeof(Stub<16>), Zone::kAlignSize) + 1;
	for (size_t i = 0; i < max; ++i) {
		auto o = new (&zone) Stub<16>();
		ASSERT_NE(nullptr, o);
		ASSERT_TRUE(o->Verify());
	}

	for (int i = 0; i < 4; ++i) {
		auto o = new (&zone) Stub<Zone::kPageSize>();
		ASSERT_NE(nullptr, o);
		ASSERT_TRUE(o->Verify());
	}
}

TEST(ZoneTest, Destroy) {
	int value = 0;
	{
		Zone zone;
		auto o = new (&zone) ScopeStub(&value);
		ASSERT_NE(nullptr, o);
		ASSERT_EQ(0, value);
	}
	ASSERT_EQ(1, value);
}

static const int kBenchmark = 1000000;

TEST(ZoneTest, Benchmark) {
	Zone zone;
	int i = kBenchmark;
	while (i--) {
		auto o = new (&zone) Stub<16>();
		ASSERT_NE(nullptr, o);
		ASSERT_TRUE(o->Verify());
	}
}

TEST(ZoneTest, DISABLED_NewBenchmark) {
	int i = kBenchmark;
	while (i--) {
		const auto size = sizeof(Stub<16>);
		auto o = new char[size];
		ASSERT_NE(nullptr, o);
	}
}

TEST(ZoneTest, Scope) {
#define zone ZoneScope::Get()
	ZoneScope top{Linker::INITIALIZER};
	ASSERT_EQ(0, ZoneScope::Current()->Nested());
	{
		ZoneScope middle{Linker::INITIALIZER};
		ASSERT_EQ(1, ZoneScope::Current()->Nested());
		auto i = 2;
		while (i--) {
			auto o = new (zone) Stub<64>;
			ASSERT_NE(nullptr, o);
			ASSERT_TRUE(o->Verify());
		}
	}
	auto o = new (zone) Stub<64>;
	ASSERT_NE(nullptr, o);
	ASSERT_TRUE(o->Verify());
#undef zone
}

} // namespace yuki

