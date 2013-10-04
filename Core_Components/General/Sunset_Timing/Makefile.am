
lib_LTLIBRARIES = libSunset_Core_Timing.la

libSunset_Core_Timing_la_SOURCES = sunset_timing.cc \
				sunset_timing.h \
				initlib.cc

libSunset_Core_Timing_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Timing_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../../Utilities/Sunset_Debug -L../../Utilities/Sunset_Utilities
libSunset_Core_Timing_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lSunset_Core_Utilities

nodist_libSunset_Core_Timing_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_timing-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Timing_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
