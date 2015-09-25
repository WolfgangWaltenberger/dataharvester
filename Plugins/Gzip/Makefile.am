AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(LIBZ_CPPFLAGS)

pkglib_LTLIBRARIES = gz.la
gz_la_SOURCES = src/GzipReader.cc include/GzipReader.h src/GzipWriter.cc include/GzipWriter.h
gz_la_LDFLAGS = -module $(LIBZ_LDFLAGS)  $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/  $(MAINLIB)
