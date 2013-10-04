
lib_LTLIBRARIES = libSunset_Core_Utilities.la

libSunset_Core_Utilities_la_SOURCES = sunset_address.cc sunset_address.h \
				  sunset_utilities.cc sunset_utilities.h \
				  initlib.cc

libSunset_Core_Utilities_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Utilities_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -L../Sunset_Debug
libSunset_Core_Utilities_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@  -lpthread -lSunset_Core_Debug

nodist_libSunset_Core_Utilities_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_utilities-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Utilities_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
