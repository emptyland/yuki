#include "yuki/at_exit.h"
#include "glog/logging.h"

namespace yuki {

struct AtExit::Hook {
	Hook *next;
	std::function<void()> func;

	Hook(const std::function<void()> &fn, Hook *p)
		: next(p)
		, func(fn) {
	}
};

AtExit *top;

AtExit::AtExit(Linker::Type)
	: chain_(top)
	, hook_(nullptr) {
	DCHECK_NE(top, this);
	top = this;
}

AtExit::~AtExit() {
	DCHECK_EQ(top, this);
	Hook *p = nullptr;
	while (hook_) {
		hook_->func();
		p = hook_;
		hook_ = hook_->next;
		delete p;
	}
	top = chain_;
}

/*static*/ AtExit *AtExit::Current() {
	return top;
}

void AtExit::AddHook(const std::function<void()> &func) {
	std::lock_guard<std::mutex> lock{mutex_};
	hook_ = new Hook{func, hook_};
}

} // namespace yuki

