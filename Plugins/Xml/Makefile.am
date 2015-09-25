AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(LIBXMLPP_CFLAGS)

pkglib_LTLIBRARIES = xml.la
xml_la_LDFLAGS = -module $(LIBXMLPP_LIBS)  $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/  $(MAINLIB)
xml_la_SOURCES = include/XmlReader.h include/XmlWriter.h src/XmlReader.cc src/XmlWriter.cc
