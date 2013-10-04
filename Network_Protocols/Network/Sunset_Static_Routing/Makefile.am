
lib_LTLIBRARIES = libSunset_Networking_Static_Routing.la

libSunset_Networking_Static_Routing_la_SOURCES = 	sunset_static_routing.cc sunset_static_routing.h \
				initlib.cc

libSunset_Networking_Static_Routing_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Networking_Static_Routing_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ -L../Sunset_Routing
libSunset_Networking_Static_Routing_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@   -lSunset_Core_Debug -lSunset_Core_Trace \
			-lSunset_Core_Utilities -lSunset_Core_Statistics -lSunset_Core_Mac_Routing \
			-lSunset_Networking_Routing -lSunset_Core_Module

nodist_libSunset_Networking_Static_Routing_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_static_routing-init.tcl 

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Static_Routing_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
