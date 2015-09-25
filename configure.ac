# Process this file with autoconf to produce a configure script.

AC_INIT(DataHarvester, 0.4.5, walten@hephy.oeaw.ac.at)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/HarvestingException.cc])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AM_MISSING_PROG(HELP2MAN, help2man, $missing_dir)

# pkg-config
AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/])
fi

if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_ERROR([*** pkg-config too old; version 0.14 or better required.])
fi


# linux time, measures real-time, not process time,
# but it does so very accurately
# works only on linux + i386
AC_ARG_ENABLE([realtime],
  [AC_HELP_STRING([--enable-realtime],[enable linux realtime timer (discouraged)])],
  [enable_realtime=$enableval], [enable_realtime=no])
AM_CONDITIONAL([HAS_REALTIME],[test "x$enable_realtime" = "xyes"])

#
plugins_enabled=""
plugins_disabled=""

AC_DEFUN([DH_PLUGIN],
[if $2 ;then
  plugins_enabled="$plugins_enabled $1"
else
  plugins_disabled="$plugins_disabled $1"
fi[]dnl
])

# plugins without 3rd party dependancies
DH_PLUGIN(Txt, [true])
DH_PLUGIN(Tnt, [true])

# tcp
AC_ARG_ENABLE( tcp, AC_HELP_STRING([--enable-tcp],[enable the tcp backend]),,
               enable_tcp="no")
DH_PLUGIN(Tcp, [test x${enable_tcp} = xyes])
AM_CONDITIONAL(HAVE_TCP, [test x${enable_tcp} = xyes])

# dh - the future dataharvester binary backend.
AC_ARG_ENABLE( dhf, AC_HELP_STRING([--enable-dhf],[enable the dh backend]),,
               enable_dhf="no")
DH_PLUGIN(Dhf, [test x${enable_dhf} = xyes])
AM_CONDITIONAL(HAVE_DHF, [test x${enable_dhf} = xyes])

# Python ...
dnl Check if python is explicitly disabled.
AC_ARG_ENABLE( python, AC_HELP_STRING([--disable-python],[disable the Python binding]),,
               enable_python="yes")
DH_PLUGIN(Python, [test x${enable_python} = xyes])
AM_CONDITIONAL(HAVE_PYTHON, [test x${enable_python} = xyes])

AC_ARG_WITH(hdf-home,
			[  --with-hdf-home=(HDF_HOME) set the full path to the hdf install directory. HDF_HOME/include needs to contain H5Cpp.h.])

HDFINCLUDE=
HDFLIB="-lhdf5_cpp -lhdf5"
if test "X$with_hdf_home" != "X"; then
  HDFINCLUDE="-I$with_hdf_home/include/"
  HDFLIB+=" -L$with_hdf_home/lib/"
fi

AC_SUBST(HDFINCLUDE)
AC_SUBST(HDFLIB)

# Docs ...
dnl Check if doc generation is explicitly disabled.
AC_ARG_ENABLE( docs, AC_HELP_STRING([--disable-docs],[disable generation of the documentation]),,
               enable_docs="yes")
AM_CONDITIONAL(HAVE_DOCS, [test x${enable_docs} = xyes])

if test x${enable_python} = xyes; then
  AM_PATH_PYTHON(2.3)
  AC_PROG_SWIG(1.3.21)
  SWIG_ENABLE_CXX
  SWIG_PYTHON
fi

# Platform-specific ldflags
PLATFORMLDFLAGS=
case "${host}" in
    *-*-cygwin)
        PLATFORMLDFLAGS="-no-undefined";;
    *-*-mingw*)
        PLATFORMLDFLAGS="-no-undefined";;
    *-*-pw32*)
        PLATFORMLDFLAGS="-no-undefined";;
esac
AC_SUBST(PLATFORMLDFLAGS)

MAINLIB=../../src/libDataHarvester.la
AC_SUBST(MAINLIB) # introduce a cycle dep , on the plugins

# Checks for libraries.
CHECK_ZLIB
AM_CONDITIONAL(HAVE_LIBZ, [test x${have_libz} = xyes])
DH_PLUGIN(Gzip, [test x${have_libz} = xyes])

# test for hdf plugin header
OLD_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $HDFINCLUDE"
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CHECK_HEADERS(H5Cpp.h, [have_hdf=yes])
AC_LANG_RESTORE
CPPFLAGS="$OLD_CPPFLAGS"

AM_CONDITIONAL(HAVE_HDF, [test x${have_hdf} = xyes])
DH_PLUGIN(Hdf, [test x${have_hdf} = xyes])

# search for ROOT
AC_SEARCH_ROOT([have_root=yes], [have_root=no])
AM_CONDITIONAL(HAVE_ROOT, [test x${have_root} = xyes])
DH_PLUGIN(Root, [test x${have_root} = xyes])

# test for hbook plugin headers
if test x${have_root} = xyes; then
 OLD_CPPFLAGS="$CPPFLAGS"
 CPPFLAGS="$CPPFLAGS $ROOTCPPFLAGS"
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_CHECK_HEADERS(THbookFile.h, [have_roothbook=yes])
 AC_LANG_RESTORE
 CPPFLAGS="$OLD_CPPFLAGS"
fi



AM_CONDITIONAL(HAVE_ROOTHBOOK, [test x${have_roothbook} = xyes])
DH_PLUGIN(HBook, [test x${have_roothbook} = xyes])

# Sqlite3
AC_ARG_ENABLE( sqlite3, AC_HELP_STRING([--disable-sqlite3],[disable the sqlite3 backend]),,
               enable_sqlite3="yes")
if test x${enable_sqlite3} = xyes; then
  PKG_CHECK_MODULES(SQLITE3, sqlite3, [have_sqlite3=yes], [:])
fi
AM_CONDITIONAL(HAVE_SQLITE3, [test x${have_sqlite3} = xyes])
DH_PLUGIN(Sqlite, [test x${have_sqlite3} = xyes])

# Xml
AC_ARG_ENABLE( xml, AC_HELP_STRING([--disable-xml],[disable the xml backend]),,
               enable_xml="yes")
if test x${enable_xml} = xyes; then
  PKG_CHECK_MODULES(LIBXMLPP, libxml++-2.6, [have_libxmlpp=yes], [:])
fi
AM_CONDITIONAL(HAVE_LIBXMLPP, [test x${have_libxmlpp} = xyes])
DH_PLUGIN(Xml, [test x${have_libxmlpp} = xyes])

# odbc
AC_ARG_ENABLE( odbc, AC_HELP_STRING([--disable-odbc],[disable the odbc backend]),,
               enable_odbc="yes")
if test x${enable_odbc} = xyes; then
  PKG_CHECK_MODULES(LIBODBCPP, [libodbc++], [have_libodbcpp=yes], [:])
fi
AM_CONDITIONAL(HAVE_LIBODBCPP, [test x${have_libodbcpp} = xyes])
DH_PLUGIN(Odbc, [test x${have_libodbcpp} = xyes])


# plugins not yet working
DH_PLUGIN(Dbf, [false])

dnl AC_CHECK_LIB([dl], [main])
dnl AC_CHECK_LIB([pthread], [main])
dnl AC_CHECK_LIB([readline], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC

AC_CONFIG_FILES([
dataharvester.pc
Makefile
src/Makefile
python/Makefile
Plugins/Makefile
Plugins/Txt/Makefile
Plugins/Dhf/Makefile
Plugins/Tnt/Makefile
Plugins/Xml/Makefile
Plugins/Gzip/Makefile
Plugins/Odbc/Makefile
Plugins/Root/Makefile
Plugins/Sqlite/Makefile
Plugins/HBook/Makefile
Plugins/Hdf/Makefile
Plugins/Tcp/Makefile
python/__init__.py
utils/Makefile
utils/python_wrapper
docs/Makefile
tests/Makefile
dataharvester.spec
])

dnl generate files
AC_OUTPUT

if test -z "$enable_maintainer_mode"; then
  enable_maintainer_mode="no"
fi

AC_MSG_RESULT(

=============
Configuration
=============
             Compiler: ${CXX}
             CXXFLAGS: ${CXXFLAGS}
              LDFLAGS: ${LDFLAGS}
       Install prefix: ${prefix}
      plugins enabled: ${plugins_enabled}
     plugins disabled: ${plugins_disabled}
       realtime timer: ${enable_realtime}
  Build documentation: ${enable_docs}
      Maintainer mode: ${enable_maintainer_mode}
)
