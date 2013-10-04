
lib_LTLIBRARIES = libSunset_Core_Queue.la

libSunset_Core_Queue_la_SOURCES = sunset_queue.cc sunset_queue.h initlib.cc

libSunset_Core_Queue_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Queue_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../../Utilities/Sunset_Debug -L../../Utilities/Sunset_Utilities -L../Sunset_Module -L../../Statistics/Sunset_Statistics
libSunset_Core_Queue_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@  -lSunset_Core_Debug -lSunset_Core_Utilities -lSunset_Core_Statistics -lSunset_Core_Module

nodist_libSunset_Core_Queue_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_queue-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Queue_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
