
lib_LTLIBRARIES = libSunset_Networking_Phy.la

libSunset_Networking_Phy_la_SOURCES = sunset_phy.cc sunset_phy.h \
							initlib.cc

libSunset_Networking_Phy_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@  -W -Wall
libSunset_Networking_Phy_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -W -Wall -L${SUNSET_LIB_FOLDER}/lib/ 
libSunset_Networking_Phy_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@   -lSunset_Core_Debug -lSunset_Core_Modem_Phy \
		-lSunset_Core_Phy_Mac -lSunset_Core_Utilities -lSunset_Core_Packet_Error_Model -lSunset_Core_Module \
		-lmphy -lSunset_Core_Common_Header

nodist_libSunset_Networking_Phy_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_phy-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Phy_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
