
lib_LTLIBRARIES = libSunset_Core_Position.la

libSunset_Core_Position_la_SOURCES = sunset_position.cc sunset_position.h initlib.cc 

libSunset_Core_Position_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Core_Position_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../Sunset_Debug -L../Sunset_Utilities -L../../General/Sunset_Module -L../Sunset_Information_Dispatcher
libSunset_Core_Position_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lSunset_Core_Utilities \
					-lSunset_Core_Information_Dispatcher -lSunset_Core_Module 


nodist_libSunset_Core_Position_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_position-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_position_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
