#include "yuki/lazy_instance.h"
#include "gmock/gmock.h"
#include <condition_variable>
#include <thread>
#include <memory>

namespace yuki {
namespace {

class Foo {
public:
	Foo() : value_(0) {}

	int Value() const { return value_; }

	void SetValue(int value) { value_ = value; }

	const Foo *Self() const { return this; }

private:
	int value_;
};

class ExitVerify {
public:
	ExitVerify() = default;

	~ExitVerify() { *value_ = 1; }

	void SetValue(int *value) { value_ = value; }

private:
	int *value_;
};

} // namespace

TEST(LazyInstanceTest, Sanity) {
	static LazyInstance<Foo> foo;
	ASSERT_EQ(foo.Get(), foo->Self());

	ASSERT_EQ(0, foo->Value());

	foo->SetValue(65535);
	ASSERT_EQ(65535, foo->Value());
}

TEST(LazyInstanceTest, AtExit) {
	int value = 0;
	{
		AtExit scoped_exit(Linker::INITIALIZER);
		static LazyInstance<ExitVerify> foo;
		foo->SetValue(&value);
	}
	ASSERT_EQ(1, value);
}

TEST(LazyInstanceTest, ThreadSafe) {
	static const int kNumThread = 20;
	std::mutex mutex;
	std::condition_variable cv;

	std::unique_ptr<std::thread[]> t{new std::thread[kNumThread]};
	for (int i = 0; i < kNumThread; ++i) {
		t[i] = std::move(std::thread{[&mutex, &cv]() {
			static LazyInstance<Foo> foo;

			std::unique_lock<std::mutex> lock(mutex);
			cv.wait(lock, [] () -> bool { return true; });
			for (int i = 0; i < 100; ++i) {
				ASSERT_EQ(foo.Get(), foo->Self());
				ASSERT_EQ(0, foo->Value());
			}
		}});
	}
	cv.notify_all();
	for (int i = 0; i < kNumThread; ++i)
		t[i].join();
}

TEST(Utils, RoundUp) {
	ASSERT_EQ(16, RoundUp(1,  16));
	ASSERT_EQ(32, RoundUp(17, 16));
	ASSERT_EQ(64, RoundUp(64, 16));
}

} // namespace yuki

