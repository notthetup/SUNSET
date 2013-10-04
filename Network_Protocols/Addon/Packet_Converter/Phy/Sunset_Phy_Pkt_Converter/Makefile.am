
lib_LTLIBRARIES = libSunset_Networking_Phy_PktConverter.la

libSunset_Networking_Phy_PktConverter_la_SOURCES = sunset_phy_pkt_converter.cc sunset_phy_pkt_converter.h \
				initlib.cc

libSunset_Networking_Phy_PktConverter_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Networking_Phy_PktConverter_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/  -L../../../../Phy/Sunset_Phy
libSunset_Networking_Phy_PktConverter_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Utilities -lSunset_Networking_Phy -lSunset_Core_PktConverter -lSunset_Core_Debug

nodist_libSunset_Networking_Phy_PktConverter_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_phy_pkt_converter-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Phy_PktConverter_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
