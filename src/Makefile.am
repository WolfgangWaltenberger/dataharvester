# -fno-strict-aliasing will be needed for Java!!
AM_CPPFLAGS = -I$(top_srcdir)/include/ -DDH_PLUGIN_DIR=\"$(pkglibdir)\"

if HAS_REALTIME                                                                                                                                
  AM_CPPFLAGS += -DFINE_GRAINED_LINUX_TIMING                                                                                                   
endif    

lib_LTLIBRARIES = libDataHarvester.la
libDataHarvester_la_LIBADD = -ldl
libDataHarvester_la_LDFLAGS = ${PLATFORMLDFLAGS}
libDataHarvester_la_SOURCES = \
  AbstractReader.cc \
  AbstractWriter.cc \
	CompositeReader.cc \
	CompositeWriter.cc \
	ConfigurationSetter.cc \
	Cout.cc \
	HarvestingException.cc \
	Helpers.cc \
	Mode.cc \
	MultiType.cc \
	NoFilter.cc \
	NoManipulator.cc \
	CommentsExpresser.cc \
	CommentsCollector.cc \
	Reader.cc \
	ReaderBuilder.cc \
	ReaderManager.cc \
	ReaderPrototypeBuilder.cc \
	SimpleConfiguration.cc \
	StreamableReader.cc \
	StreamableWriter.cc \
	StringTools.cc \
	SystemWriter.cc \
	TerminateException.cc \
	Tuple.cc \
	TupleNameFilter.cc \
	TupleRow.cc \
	TupleRowConsistencyChecker.cc \
	UniversalManipulator.cc \
	Writer.cc \
	Timer.cc \
	WriterBuilder.cc \
	WriterManager.cc
