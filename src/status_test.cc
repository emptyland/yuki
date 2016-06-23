#include "yuki/status.h"
#include "gmock/gmock.h"

namespace {

class Moveable {
public:
	explicit Moveable(int val) : val_(val) {}

	Moveable(Moveable &&other) : val_(other.val_) {
		other.val_ = 0;
	}

	~Moveable() {}

	int Value() const { return val_; }

private:
	Moveable(const Moveable &) = delete;
	void operator = (const Moveable &) = delete;

	int val_;
};

} // namespace

namespace yuki {

TEST(StatusTest, Moveable) {
	Moveable old(1);

	ASSERT_EQ(1, old.Value());

	Moveable obj(std::move(old));

	ASSERT_EQ(0, old.Value());
	ASSERT_EQ(1, obj.Value());
}

TEST(StatusTest, Sanity) {
	Status s(Status::Errorf(Status::kNotFound, "No key be found"));

	ASSERT_EQ(Status::kNotFound, s.Code());

	Status err = std::move(s);
	ASSERT_EQ(Status::kNotFound, err.Code());

	err = Status::Errorf(Status::kSystemError, "No error");
	ASSERT_EQ(Status::kSystemError, err.Code());
}

} // namespace yuki

