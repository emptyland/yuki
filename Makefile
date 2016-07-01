THIRD_PARTY_LIBS=third_party/libs/libgflags.a \
                 third_party/libs/libgflags_nothreads.a \
                 third_party/libs/libglog.a \
                 third_party/libs/libgtest.a \
                 third_party/libs/libgtest_main.a \
                 third_party/libs/libpcrecpp.a \
                 third_party/libs/libpcre.a \
                 third_party/libs/libpcre16.a \
                 third_party/libs/libpcre32.a \

all: src/libyuki.a src/yuki_test

src/yuki_test src/libyuki.a: $(THIRD_PARTY_LIBS)
	cd src && $(MAKE) all

$(THIRD_PARTY_LIBS):
	CDEP_PATH=cdep python cdep/cdep.py

.PHONY: clean distclean

clean:
	cd src && $(MAKE) $@

distclean:
	cd src && $(MAKE) $@

