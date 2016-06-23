#ifndef YUKI_LAZY_INSTANCE_H
#define YUKI_LAZY_INSTANCE_H

#include <atomic>
#include <thread>
#include <stdint.h>
#include "yuki/at_exit.h"

namespace yuki {

template<class T>
class DefaultLazyPolicy {
public:
	static T *New(void *chunk) {
		return new (chunk) T;
	}

	static void Delete(T *obj) {
		obj->~T();
	}

private:
	DefaultLazyPolicy() = delete;
	DefaultLazyPolicy(const DefaultLazyPolicy &) = delete;
	void operator = (const DefaultLazyPolicy &) = delete;
};

template<class T, class Policy = DefaultLazyPolicy<T>>
class LazyInstance {
	static const uintptr_t kPendingMask = 1;
	static const uintptr_t kCreatedMask = ~kPendingMask;

public:
	LazyInstance() : inst_(0) {}

	T *Get() {
		if (!(std::atomic_load_explicit(&inst_,
						std::memory_order_acquire) & kCreatedMask) &&
				NeedInit())
			Install();
		return Instance();
	}

	T *operator -> () {
		return Get();
	}

private:
	LazyInstance(const LazyInstance &) = delete;
	void operator = (const LazyInstance &) = delete;

	bool NeedInit() {
		uintptr_t exp = 0;
		if (std::atomic_compare_exchange_weak(&inst_, &exp, kPendingMask))
			return true;
		while (std::atomic_load_explicit(&inst_,
					std::memory_order_acquire) == kPendingMask)
			std::this_thread::yield();
		return false;
	}

	void Install() {
		union { uintptr_t to; T *from; };
		from = Policy::New(body_);
		AtExit::Current()->AddHook(
				std::bind(&Policy::Delete, from));
		std::atomic_store_explicit(&inst_, to, std::memory_order_release);
	}

	T *Instance() {
		union { T *to; uintptr_t from; };
		from = std::atomic_load(&inst_);
		return to;
	}

	std::atomic<uintptr_t> inst_;
	char body_[sizeof(T)];
}; // class LazyInstance

} // namespace yuki

#endif //YUKI_LAZY_INSTANCE_H

