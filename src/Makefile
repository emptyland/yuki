OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')

CC=clang
CXX=clang++
CFLAGS=-I../third-party/include -I. -Wall -Wextra
CXXFLAGS=-std=c++11 -I../third-party/include -I. -Wall -Wextra

ifeq ($(OS), Linux)
	LIBS=-lpthread -lunwind -L../third-party/libs -lglog -lpcre -lgmock
else
	LIBS=-lpthread -L../third-party/libs -lglog -lpcre -lgmock
endif

ifeq ($(DEBUG), 1)
	CFLAGS+=-O0 -g -DDEBUG
	CXXFLAGS+=-O0 -g -DDEBUG
else
	CFLAGS+=-O2
	CXXFLAGS+=-O2
endif

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
          zone_test.o

all: yuki_test libyuki.a
	./yuki_test

yuki_test: $(TEST_OBJS) $(OBJS)
	$(CXX) $(OBJS) $(TEST_OBJS) -o yuki_test $(LIBS)

libyuki.a: $(OBJS)
	ar rs libyuki.a $(OBJS)

.PHONY: clean distclean

distclean: clean
	rm -r ../third-party

clean:
	rm -f $(OBJS) $(TEST_OBJS) libyuki.a yuki_test


