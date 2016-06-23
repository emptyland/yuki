#include "yuki/zone.h"
#include "yuki/utils.h"
#include <stdlib.h>

namespace yuki {

void *Zone::New(size_t size) {
	size_t i, s = kAlignSize;
	size = RoundUp(size, kAlignSize);
	for (i = 0; i < sizeof(s) * 8; ++i) {
		if (size < s)
			break;
		s <<= 1;
	}
	if (i >= kMaxSlots - 1)
		return NewLarge(size);
	// allocate small chunk
	size = kAlignSize << i;
	Segment *pos = slot_[i];
	if (!pos) {
		slot_[i] = NewSegment(pos, kPageSize);
		pos = slot_[i];
	}
	void *x = NewSmall(size, pos);
	if (x)
		return x;
	slot_[i] = NewSegment(pos, kPageSize);
	pos = slot_[i];
	return NewSmall(size, pos);
}

void Zone::DeleteAll(bool will_destroy) {
	const size_t max = kMaxSlots - 1;
	for (size_t i = 0; i < max; ++i) {
		auto size = kAlignSize << i;
		while (slot_[i]) {
			auto pos = slot_[i], next = pos->Next();
			if (will_destroy)
				DeleteSegment(pos, size);
			else
				free(pos);
			slot_[i] = next;
		}
	}
	// One large segment is one object
	while (slot_[max]) {
		auto pos = slot_[max], next = pos->Next();
		if (will_destroy)
			bit_cast<ZoneObject*>(pos->Start())->~ZoneObject();
		free(pos);
		slot_[max] = next;
	}
}

static __thread ZoneScope *top;

ZoneScope::ZoneScope(Linker::Type)
	: chain_(top)
	, nested_(!chain_ ? 0 : chain_->nested_ + 1) {
	DCHECK_NE(top, this);
	top = this;
}

ZoneScope::~ZoneScope() {
	DCHECK_EQ(top, this);
	top = chain_;
}

/*static*/ ZoneScope *ZoneScope::Current() {
	return DCHECK_NOTNULL(top);
}


} // namespace yuki

