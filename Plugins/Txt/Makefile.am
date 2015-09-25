AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/

pkglib_LTLIBRARIES = txt.la
txt_la_SOURCES = src/TextReader.cc include/TextReader.h src/TextWriter.cc include/TextWriter.h
txt_la_LDFLAGS = -module  $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/ $(MAINLIB)
