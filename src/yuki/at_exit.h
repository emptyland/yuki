#ifndef YUKI_AT_EXIT_H
#define YUKI_AT_EXIT_H

#include "yuki/utils.h"
#include <mutex>
#include <functional>

namespace yuki {

class AtExit {
public:
	explicit AtExit(Linker::Type);

	~AtExit();

	static AtExit *Current();

	void AddHook(const std::function<void()> &hook);

private:
	struct Hook;

	AtExit(const AtExit &) = delete;
	void operator = (const AtExit &) = delete;

	AtExit *chain_;
	Hook *hook_;
	std::mutex mutex_;
}; // class AtExit

} // namespace yuki

#endif //YUKI_AT_EXIT_H

