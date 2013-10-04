
lib_LTLIBRARIES = libSunset_Networking_Protocol_Statistics.la

libSunset_Networking_Protocol_Statistics_la_SOURCES = sunset_protocol_statistics.cc sunset_protocol_statistics.h \
				 initlib.cc

libSunset_Networking_Protocol_Statistics_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Networking_Protocol_Statistics_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ -L../../../Application/Sunset_Agent -L../../../Datalink/Sunset_Mac -L../../../Network/Sunset_Routing -L../../../Phy/Sunset_Phy
libSunset_Networking_Protocol_Statistics_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lmiraclecbr \
				-lSunset_Core_Utilities -lSunset_Networking_Mac -lSunset_Core_Statistics -lSunset_Networking_Agent \
				-lSunset_Networking_Phy -lSunset_Networking_Routing -lSunset_Core_Common_Header -lSunset_Core_Trace

nodist_libSunset_Networking_Protocol_Statistics_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_protocol_statistics-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Protocol_Statistics_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
