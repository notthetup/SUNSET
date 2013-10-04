
lib_LTLIBRARIES = libSunset_Core_Phy_Mac.la

libSunset_Core_Phy_Mac_la_SOURCES = sunset_phy2mac-clmsg.cc sunset_phy2mac-clmsg.h \
							initlib.cc

libSunset_Core_Phy_Mac_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Phy_Mac_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ -L../../Utilities/Sunset_Debug
libSunset_Core_Phy_Mac_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lmphy

nodist_libSunset_Core_Phy_Mac_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_phy2mac-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Phy_Mac_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
