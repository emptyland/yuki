CC=gcc
CXX=g++
CFLAGS=-I../third-party/include -I. -O3 -Wall -Wextra
CXXFLAGS=-std=c++11 -I../third-party/include -I. -O3 -Wall -Wextra

OBJS=at_exit.o \
	 file_path.o \
	 file_posix.o \
	 status.o \
	 strings.o \
	 varint.o \
	 zone.o

TEST_OBJS=all_test.o \
          at_exit_test.o \
          file_test.o \
          lazy_instance_test.o \
          skiplist_test.o \
          slice_test.o \
          status_test.o \
          strings_test.o \
          varint_test.o \
          zone_test.o

all: yuki_test
	./yuki_test

yuki_test: libyuki.a $(TEST_OBJS)
	$(CXX) -L. -lyuki -L../third-party/libs -lgmock -lglog -lgflags $(TEST_OBJS) -o yuki_test

libyuki.a: $(OBJS)
	ar rs libyuki.a $(OBJS)

.PHONY: clean distclean

distclean: clean
	rm -r ../third-party

clean:
	rm -f $(OBJS) libyuki.a yuki_test


