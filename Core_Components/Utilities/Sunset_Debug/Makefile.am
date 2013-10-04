
lib_LTLIBRARIES = libSunset_Core_Debug.la

libSunset_Core_Debug_la_SOURCES = sunset_debug.cc sunset_debug.h \
				 initlib.cc

libSunset_Core_Debug_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Debug_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ 
libSunset_Core_Debug_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ 

nodist_libSunset_Core_Debug_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_debug-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Debug_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
