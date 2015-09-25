AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(ROOTCPPFLAGS)

pkglib_LTLIBRARIES = hbook.la
hbook_la_LDFLAGS = -module -L/$(ROOTLIBDIR) $(ROOTLIBS) $(PLATFORMLDFLAGS)  -L$(top_srcdir)/src/.libs/  $(MAINLIB)
hbook_la_SOURCES = include/HBookReader.h src/HBookReader.cc
