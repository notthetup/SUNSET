
lib_LTLIBRARIES = libSunset_Networking_Flooding.la

libSunset_Networking_Flooding_la_SOURCES = 	sunset_flooding.cc sunset_flooding.h \
				initlib.cc

libSunset_Networking_Flooding_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Networking_Flooding_la_LDFLAGS =  @NS_LDFLAGS@  @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ -L../Sunset_Routing 
libSunset_Networking_Flooding_la_LIBADD =   @NS_LIBADD@  @NSMIRACLE_LIBADD@ -lpthread -lSunset_Core_Debug -lSunset_Core_Trace \
		-lSunset_Core_Utilities -lSunset_Networking_Routing -lSunset_Core_Mac_Routing \
		-lSunset_Core_Module -lSunset_Core_Statistics

nodist_libSunset_Networking_Flooding_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_flooding-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Flooding_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
