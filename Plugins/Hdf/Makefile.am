AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(HDFINCLUDE)

pkglib_LTLIBRARIES = hdf.la
hdf_la_LDFLAGS = -module $(HDFLIB) $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/ $(MAINLIB)
hdf_la_SOURCES = include/HdfReader.h include/HdfWriter.h include/HdfHelpers.h \
				 src/HdfHelpers.cc src/HdfReader.cc src/HdfWriter.cc
