#include "yuki/at_exit.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "glog/logging.h"

int main(int argc, char *argv[]) {
	google::InitGoogleLogging(argv[0]);
	testing::InitGoogleMock(&argc, argv);
	yuki::AtExit scoped_exit(yuki::Linker::INITIALIZER);
	return RUN_ALL_TESTS();
}

