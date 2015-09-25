AM_CPPFLAGS = -I$(srcdir)/../include/

# tools run during "make check" and "make distcheck"
TESTS = MultiTypeTest

check_PROGRAMS = MultiTypeTest

MultiTypeTest_SOURCES = MultiTypeTest.cpp
MultiTypeTest_LDADD = ../src/libDataHarvester.la
MultiTypeTest_LDFLAGS = ${PLATFORMLDFLAGS}

test:
	./test_suite.sh
