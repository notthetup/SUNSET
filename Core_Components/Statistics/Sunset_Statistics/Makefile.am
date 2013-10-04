
lib_LTLIBRARIES = libSunset_Core_Statistics.la

libSunset_Core_Statistics_la_SOURCES = sunset_statistics.cc sunset_statistics.h \
				 initlib.cc

libSunset_Core_Statistics_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Statistics_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../../Utilities/Sunset_Debug -L../../Utilities/Sunset_Utilities
libSunset_Core_Statistics_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@  -lSunset_Core_Debug -lSunset_Core_Utilities

nodist_libSunset_Core_Statistics_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_statistics-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Statistics_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
