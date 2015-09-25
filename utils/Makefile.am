PYTHON_TOOLS=dataharvester_merge dataharvester_mkhierarchic dataharvester_systemwriter dataharvester_converter

${PYTHON_TOOLS}: python_wrapper
	cp $< $@
BUILT_SOURCES = ${PYTHON_TOOLS}
bin_SCRIPTS = ${PYTHON_TOOLS}

pythonlibdir=$(libdir)/python
pythonlib_SCRIPTS=dataharvester_merge.py dataharvester_mkhierarchic.py dataharvester_systemwriter.py dataharvester_converter.py

CLEANFILES = ${PYTHON_TOOLS}
EXTRA_DIST = dataharvester_merge.py dataharvester_mkhierarchic.py dataharvester_systemwriter.py dataharvester_converter.py
