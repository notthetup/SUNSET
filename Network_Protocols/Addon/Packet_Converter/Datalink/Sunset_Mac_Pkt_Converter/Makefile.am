
lib_LTLIBRARIES = libSunset_Networking_Mac_PktConverter.la

libSunset_Networking_Mac_PktConverter_la_SOURCES = sunset_mac_pkt_converter.cc sunset_mac_pkt_converter.h \
				initlib.cc

libSunset_Networking_Mac_PktConverter_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Networking_Mac_PktConverter_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/  -L../../../../Datalink/Sunset_Mac
libSunset_Networking_Mac_PktConverter_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lSunset_Core_Utilities \
			-lSunset_Core_PktConverter -lSunset_Networking_Mac

nodist_libSunset_Networking_Mac_PktConverter_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_mac_pkt_converter-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Mac_PktConverter_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
