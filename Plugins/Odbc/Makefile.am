AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(LIBODBCPP_CFLAGS)

pkglib_LTLIBRARIES = odbc.la
odbc_la_LDFLAGS = -module $(LIBODBCPP_LIBS)   $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/  $(MAINLIB)
odbc_la_SOURCES = include/OdbcWriter.h src/OdbcWriter.cc
