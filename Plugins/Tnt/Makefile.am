AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/

pkglib_LTLIBRARIES = tnt.la
tnt_la_LDFLAGS = -module  $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/  $(MAINLIB)
tnt_la_SOURCES = include/TntWriter.h src/TntWriter.cc 
