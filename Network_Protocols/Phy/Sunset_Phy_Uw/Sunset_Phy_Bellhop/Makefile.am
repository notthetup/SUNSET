if HAVE_WOSS
lib_LTLIBRARIES = libSunset_Networking_Phy_Bellhop.la

libSunset_Networking_Phy_Bellhop_la_SOURCES = sunset_phy_bellhop.cc sunset_phy_bellhop.h \
				initlib.cc

libSunset_Networking_Phy_Bellhop_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ @WOSS_CPPFLAGS@
libSunset_Networking_Phy_Bellhop_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/
libSunset_Networking_Phy_Bellhop_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lSunset_Core_Utilities \
		-lSunset_Core_Information_Dispatcher -lSunset_Core_Phy_Mac -lSunset_Core_Energy_Model \
		-lSunset_Core_Packet_Error_Model -lSunset_Core_Module -lWOSS -lWOSSPhy

nodist_libSunset_Networking_Phy_Bellhop_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_phy_bellhop-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Phy_Bellhop_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
endif