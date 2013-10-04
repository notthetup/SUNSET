
lib_LTLIBRARIES = libSunset_Core_Trace.la

libSunset_Core_Trace_la_SOURCES = sunset_trace.cc sunset_trace.h \
				 initlib.cc

libSunset_Core_Trace_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Trace_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -L../Sunset_Debug
libSunset_Core_Trace_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug

nodist_libSunset_Core_Trace_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_trace-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Trace_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
