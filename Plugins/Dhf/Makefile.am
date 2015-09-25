AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/

pkglib_LTLIBRARIES = dhf.la
dhf_la_SOURCES = src/DhfReader.cc include/DhfReader.h src/DhfWriter.cc include/DhfWriter.h src/MultiTypeBinaryConverter.cc include/MultiTypeBinaryConverter.h src/DhfHelpers.cc include/DhfHelpers.h
dhf_la_LDFLAGS = -module  $(PLATFORMLDFLAGS) $(MAINLIB)
