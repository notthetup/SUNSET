
lib_LTLIBRARIES = libSunset_Networking_Transport.la

libSunset_Networking_Transport_la_SOURCES = 	sunset_transport.cc sunset_transport.h \
				initlib.cc

libSunset_Networking_Transport_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Networking_Transport_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ 
libSunset_Networking_Transport_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@   -lSunset_Core_Debug -lSunset_Core_Utilities \
		-lSunset_Core_Statistics -lSunset_Core_Module

nodist_libSunset_Networking_Transport_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_transport-init.tcl  

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Transport_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
