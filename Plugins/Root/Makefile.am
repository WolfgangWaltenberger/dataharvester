AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(ROOTCPPFLAGS)

pkglib_LTLIBRARIES = root.la
root_la_LDFLAGS = -module -L/$(ROOTLIBDIR) $(ROOTLIBS) -lTreePlayer $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/  $(MAINLIB)
root_la_SOURCES = src/RootDataCache.cc src/RootInternalNtuple.cc src/RootReader.cc src/RootWriter.cc \
				  src/RootHelpers.cc include/RootDataCache.h include/RootInternalNtuple.h \
				  include/RootReader.h  include/RootWriter.h include/RootHelpers.h
