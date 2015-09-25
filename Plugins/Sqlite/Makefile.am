AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ $(SQLITE3_CFLAGS)

pkglib_LTLIBRARIES = sqlite.la
sqlite_la_LDFLAGS = -module $(SQLITE3_LIBS)   $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/  $(MAINLIB)
sqlite_la_SOURCES = include/SqliteReader.h include/SqliteWriter.h src/SqliteReader.cc src/SqliteWriter.cc
