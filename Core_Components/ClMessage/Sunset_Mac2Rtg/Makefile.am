
lib_LTLIBRARIES = libSunset_Core_Mac_Routing.la

libSunset_Core_Mac_Routing_la_SOURCES = sunset_mac2rtg-clmsg.cc sunset_mac2rtg-clmsg.h \
				initlib.cc

libSunset_Core_Mac_Routing_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Core_Mac_Routing_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -L../../Utilities/Sunset_Debug
libSunset_Core_Mac_Routing_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug

nodist_libSunset_Core_Mac_Routing_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_mac2rtg-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Mac_Routing_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
