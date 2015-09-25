ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src Plugins utils tests
if HAVE_PYTHON
  SUBDIRS += python
endif

if HAVE_DOCS
  SUBDIRS += docs
endif

# rules for pkg-config
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dataharvester.pc

$(pkgconfig_DATA): config.status

# include headers
pkginclude_HEADERS = \
	include/dataharvester/AbstractReader.h \
	include/dataharvester/AbstractWriter.h \
	include/dataharvester/CompositeReader.h \
	include/dataharvester/CompositeWriter.h \
	include/dataharvester/ConfigurationParser.h \
	include/dataharvester/ConfigurationSetter.h \
	include/dataharvester/Cout.h \
	include/dataharvester/Filter.h \
	include/dataharvester/TupleNameFilter.h \
	include/dataharvester/NoFilter.h \
	include/dataharvester/HarvestingConfiguration.h \
	include/dataharvester/HarvestingException.h \
	include/dataharvester/Helpers.h \
	include/dataharvester/Manipulator.h \
	include/dataharvester/NoManipulator.h \
	include/dataharvester/UniversalManipulator.h \
	include/dataharvester/CommentsExpresser.h \
	include/dataharvester/CommentsCollector.h \
	include/dataharvester/Mode.h \
	include/dataharvester/MultiType.h \
	include/dataharvester/ReaderBuilder.h \
	include/dataharvester/Reader.h \
	include/dataharvester/ReaderManager.h \
	include/dataharvester/ReaderPrototypeBuilder.h \
	include/dataharvester/SimpleConfigurable.h \
	include/dataharvester/SimpleConfiguration.h \
	include/dataharvester/StreamableReader.h \
	include/dataharvester/StreamableWriter.h \
	include/dataharvester/StringTools.h \
	include/dataharvester/SystemWriter.h \
	include/dataharvester/TerminateException.h \
	include/dataharvester/Tuple.h \
	include/dataharvester/TupleRowConsistencyChecker.h \
	include/dataharvester/TupleRow.h \
	include/dataharvester/WriterBuilder.h \
	include/dataharvester/Writer.h \
	include/dataharvester/Timer.h \
	include/dataharvester/WriterManager.h \
	include/dataharvester/WriterPrototypeBuilder.h

hepforge_distupload: dist
	scp dataharvester-${VERSION}.tar.gz login.hepforge.org:dataharvester/downloads

