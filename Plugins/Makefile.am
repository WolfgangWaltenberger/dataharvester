# don't need additional libraries
SUBDIRS = Txt Tnt

if HAVE_DHF
  SUBDIRS += Dhf
endif

# no extra dependencies, but can be turned off
if HAVE_TCP
  SUBDIRS += Tcp
endif

# depend on additional libraries
if HAVE_LIBXMLPP
  SUBDIRS += Xml
endif
if HAVE_LIBZ
  SUBDIRS += Gzip
endif
if HAVE_ROOT
  SUBDIRS += Root
endif
if HAVE_ROOTHBOOK
  SUBDIRS += HBook
endif
if HAVE_SQLITE3
  SUBDIRS += Sqlite
endif
if HAVE_LIBODBCPP
  SUBDIRS += Odbc
endif
if HAVE_HDF
  SUBDIRS += Hdf
endif

#SUBDIRS += Dbf
