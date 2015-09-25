BUILT_SOURCES = $(srcdir)/dataharvester.cc
SWIG_SOURCES = dataharvester.i
## DESTDIR="/" ## $(pkgpythondir)

pkgpython_PYTHON = dataharvester.py __init__.py
pkgpyexec_LTLIBRARIES = _dataharvester.la

_dataharvester_la_SOURCES = $(srcdir)/dataharvester.cc $(SWIG_SOURCES)
_dataharvester_la_CPPFLAGS = $(SWIG_PYTHON_CPPFLAGS) -I$(top_srcdir)/include/
_dataharvester_la_LDFLAGS = -module -lpython${PYTHON_VERSION} ${PLATFORMLDFLAGS} -L$(top_srcdir)/src/.libs/ 
_dataharvester_la_LIBADD = ../src/libDataHarvester.la

.deps/_dataharvester_la-dataharvester.Plo:
	mkdir -p $(srcdir)/.deps/
	touch $(srcdir)/.deps/_dataharvester_la-dataharvester.Plo

$(srcdir)/dataharvester.cc $(srcdir)/dataharvester.py: $(SWIG_SOURCES)
	mkdir -p $(srcdir)/.deps/
	touch $(srcdir)/.deps/_dataharvester_la-dataharvester.Plo
	$(SWIG) $(SWIG_PYTHON_OPT) -I$(top_srcdir)/include -o $(srcdir)/dataharvester.cc $<

CLEANFILES = $(srcdir)/dataharvester.cc $(srcdir)/dataharvester.py $(srcdir)/_dataharvester_la-dataharvester.loT $(srcdir)/.deps/_dataharvester_la-dataharvester.Plo $(srcdir)
