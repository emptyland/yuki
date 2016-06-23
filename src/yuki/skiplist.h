#ifndef YUKI_SKIPLIST_H
#define YUKI_SKIPLIST_H

#include "yuki/zone.h"
#include "glog/logging.h"
#include <atomic>
#include <random>
#include <functional>

namespace yuki {
namespace internal {

class Random {
public:
	Random(int min, int max)
		: dist_(min, max)
		, generator_(std::bind(dist_, engine_)) {
	}

	unsigned Next() { return generator_(); }

private:
	std::uniform_int_distribution<unsigned> dist_;
	std::mt19937 engine_;
	std::function<unsigned()> generator_;
}; // class Random

} // namespace internal

template<class K, class Pred>
class SkipList {
public:
	struct Node;
	class  Iterator;

	static const int kMaxHeight = 17;

	SkipList(Pred pred, Zone *zone);

	void Insert(const K &key);

	int MaxHeight() const;

private:
	SkipList(const SkipList &) = delete;
	void operator = (const SkipList &) = delete;

	int RandomHeight();

	// Comparing:
	bool KeyIsAfterNode(const K &key, Node *node) const;

	// Finding:
	Node *FindGreaterOrEqual(const K &key, Node **prev) const;

	Pred const pred_;
	Zone * const zone_;
	Node * const head_;
	std::atomic_int max_height_;
	internal::Random rand_;
}; // class SkipList

//-----------------------------------------------------------------------------
template<class K, class Pred>
SkipList<K, Pred>::SkipList(Pred pred, Zone *zone)
	: pred_(pred)
	, zone_(zone)
	, head_(Node::New(zone, 0, kMaxHeight))
	, max_height_(1)
	, rand_(0U, 0xffffffffU) {
	for (int i = 0; i < kMaxHeight; ++i)
		head_->SetNext(i, nullptr);
}

template<class K, class Pred>
int SkipList<K, Pred>::MaxHeight() const {
	return max_height_.load(std::memory_order_relaxed);
}

template<class K, class Pred>
void SkipList<K, Pred>::Insert(const K &key) {
	Node *prev[kMaxHeight];
	Node *x = FindGreaterOrEqual(key, prev);

	DCHECK((x == nullptr) || pred_(key, x->key));

	int height = RandomHeight();
	if (height > MaxHeight()) {
		for (int i = MaxHeight(); i < height; ++i)
			prev[i] = head_;
		max_height_.store(height, std::memory_order_relaxed);
	}

	x = Node::New(zone_, key, height);
	for (int i = 0; i < height; ++i) {
		x->NoBarrierSetNext(i, prev[i]->NoBarrierNext(i));
		prev[i]->SetNext(i, x);
	}
}

template<class K, class Pred>
int SkipList<K, Pred>::RandomHeight() {
	static const unsigned kBranching = 4U;
	int height = 1;
	while (height < kMaxHeight && ((rand_.Next() % kBranching) == 0))
		++height;
	DCHECK_GT(height, 0);
	DCHECK_LE(height, kMaxHeight);
	return height;
}

template<class K, class Pred>
typename SkipList<K, Pred>::Node *
SkipList<K, Pred>::FindGreaterOrEqual(const K &key, Node **prev) const {
	Node *x = head_;
	int level = MaxHeight() - 1;
	while (1) {
		Node *next = x->Next(level);
		if (KeyIsAfterNode(key, next)) {
			x = next;
		} else {
			if (prev != nullptr) prev[level] = x;
			if (level == 0) {
				return next;
			} else {
				--level;
			}
		}
	}
}

template<class K, class Pred>
bool SkipList<K, Pred>::KeyIsAfterNode(const K &key, Node *node) const {
	return (node != nullptr) && (pred_(node->key, key) < 0);
}

//-----------------------------------------------------------------------------
template<class K, class Pred>
struct SkipList<K, Pred>::Node {

	explicit Node(const K &k) : key(k) {}

	Node *Next(int n) {
		DCHECK_GE(n, 0);
		return next_[n].load(std::memory_order_acquire);
	}

	void SetNext(int n, Node *x) {
		DCHECK_GE(n, 0);
		next_[n].store(x, std::memory_order_release);
	}

	Node *NoBarrierNext(int n) {
		DCHECK_GE(n, 0);
		return next_[n].load(std::memory_order_relaxed);
	}

	void NoBarrierSetNext(int n, Node *x) {
		DCHECK_GE(n, 0);
		next_[n].store(x, std::memory_order_relaxed);
	}

	static Node *New(Zone *zone, const K &key, int height) {
		void *chunk = zone->New(sizeof(Node)
				+ sizeof(std::atomic<Node*>) * height - 1);
		return new (chunk) Node(key);
	}

	K const key;

private:
	std::atomic<Node*> next_[1];
}; // struct Node

//-----------------------------------------------------------------------------
template<class K, class Pred>
class SkipList<K, Pred>::Iterator {
public:
	Iterator(const SkipList *list)
		: list_(list)
		, iter_(list_->head_->Next(0)) {
	}

	const K &Key() const {
		DCHECK_NOTNULL(curr_);
		return curr_->key;
	}

	K *MutableKey() {
		DCHECK_NOTNULL(curr_);
		return &curr_->key;
	}

	bool Next() {
		if (!iter_)
			return false;
		curr_ = iter_;
		iter_ = iter_->Next(0);
		return true;
	}

	bool Seek(const K &key) {
		iter_ = list_->FindGreaterOrEqual(key, nullptr);
		return Next();
	}

	void SeekToFirst() {
		iter_ = list_->head_->Next(0);
		curr_ = nullptr;
	}

	const SkipList *list_;
	Node *iter_;
	Node *curr_ = nullptr;
}; // class Iterator

} // namespace yuki

#endif //YUKI_SKIPLIST_H

