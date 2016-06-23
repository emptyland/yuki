#ifndef YUKI_ZONE_H
#define YUKI_ZONE_H

#include "yuki/utils.h"
#include "glog/logging.h"
#include <memory>
#include <stdlib.h>
#include <stdint.h>

namespace yuki {

class Segment;

class Zone {
public:
	static const size_t kMaxSlots = 7;
	static const size_t kPageSize = 8 * 1024;
	static const size_t kAlignSize = 16;

	inline Zone() = default;
	inline explicit Zone(bool no_destroy) : no_destroy_(no_destroy) {}
	~Zone() { DeleteAll(!no_destroy_); }

	void *New(size_t size);
	void DeleteAll(bool will_destroy);

private:
	Zone(const Zone &) = delete;
	void operator = (const Zone &) = delete;

	inline void *NewLarge(size_t size);
	inline void *NewSmall(size_t size, Segment *segment);
	inline Segment *NewSegment(Segment *next, size_t size);
	inline void DeleteSegment(Segment *segment, size_t size);

	// Will destroy?
	bool no_destroy_ = false;

	// Page = 8k;
	// [0]: 16
	// [1]: 32
	// [2]: 64
	// [3]: 128
	// [4]: 256
	// [5]: 512
	// [6]: >= 8k
	Segment *slot_[kMaxSlots] = {0};
}; // class Zone

class Segment {
public:
	void Init(Segment *next, size_t size) {
		next_ = next;
		size_ = size;
	}

	Segment *Next() const { return next_; }
	size_t Size() const { return size_; }
	size_t Capacity() const { return size_ - sizeof(Segment); }

	uint8_t *Start() { return Address(sizeof(Segment)); }
	uint8_t *End() { return Address(size_); }

	void Advance(size_t s) { DCHECK_LT(s, Capacity()); size_ -= s; }

private:
	Segment() = delete;
	Segment(const Segment &) = delete;
	~Segment() = delete;
	void operator = (const Segment &) = delete;

	uint8_t *Address(size_t n) {
		return bit_cast<uint8_t*>(this) + n;
	}

	Segment *next_;
	size_t   size_;
};

class ZoneObject {
public:
	virtual ~ZoneObject() = default;

	void *operator new (size_t size, Zone *zone) {
		return zone->New(size);
	}
};

class ZoneScope {
public:
	explicit ZoneScope(Linker::Type);
	~ZoneScope();

	Zone *GetZone() { return &zone_; }
	int Nested() const { return nested_; }

	static ZoneScope *Current();
	static Zone *Get() { return Current()->GetZone(); }

private:
	ZoneScope(const ZoneScope &) = delete;
	void operator = (const ZoneScope &) = delete;
	void *operator new (size_t) = delete;
	void  operator delete (void *) = delete;

	ZoneScope *chain_;
	int nested_;
	Zone zone_;
};

inline Segment *Zone::NewSegment(Segment *next, size_t size) {
	auto segment = static_cast<Segment*>(malloc(size));
	segment->Init(next, size);
	return segment;
}

inline void *Zone::NewLarge(size_t size) {
	auto pos = &slot_[kMaxSlots - 1];
	*pos = NewSegment(pos[0], size + sizeof(Segment));
	return pos[0]->Start();
}

inline void *Zone::NewSmall(size_t size, Segment *segment) {
	if (segment->Capacity() < size)
		return nullptr;
	segment->Advance(size); return segment->End();
}

inline void Zone::DeleteSegment(Segment *segment, size_t size) {
	auto end = bit_cast<uint8_t*>(segment) + kPageSize;
	for (auto i = segment->End(); i < end; i += size) {
		bit_cast<ZoneObject*>(i)->~ZoneObject();
	}
	free(segment);
}

} // namespace yuki

#endif //YUKI_ZONE_H

