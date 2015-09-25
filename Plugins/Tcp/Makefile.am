AM_CPPFLAGS = -I$(top_srcdir)/include/ -I$(srcdir)/include/ -I$(top_srcdir)/Plugins/Txt/include/

pkglib_LTLIBRARIES = tcp.la
tcp_la_LDFLAGS = -module  -lpthread $(PLATFORMLDFLAGS) -L$(top_srcdir)/src/.libs/ $(MAINLIB)
tcp_la_SOURCES = \
	include/TcpCommServer.h \
	include/TcpHelpers.h \
	include/TcpDataServerThread.h \
	include/TcpConnector.h \
	include/TcpWriter.h \
	include/TcpDataServer.h \
	src/TcpDataServerThread.cc \
	src/TcpConnector.cc \
	src/TcpWriter.cc \
	src/TcpDataServer.cc \
	src/TcpCommServer.cc \
	src/TcpHelpers.cc
