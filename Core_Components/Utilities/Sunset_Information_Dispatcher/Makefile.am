
lib_LTLIBRARIES = libSunset_Core_Information_Dispatcher.la

libSunset_Core_Information_Dispatcher_la_SOURCES = sunset_information_dispatcher.cc sunset_information_dispatcher.h initlib.cc

libSunset_Core_Information_Dispatcher_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Core_Information_Dispatcher_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../Sunset_Debug -L../Sunset_Utilities
libSunset_Core_Information_Dispatcher_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lpthread  -lSunset_Core_Debug -lSunset_Core_Utilities

nodist_libSunset_Core_Information_Dispatcher_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_information_dispatcher-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Information_Dispatcher_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
