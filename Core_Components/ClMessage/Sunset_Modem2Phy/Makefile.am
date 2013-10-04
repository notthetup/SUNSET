
lib_LTLIBRARIES = libSunset_Core_Modem_Phy.la

libSunset_Core_Modem_Phy_la_SOURCES =  sunset_modem2phy-clmsg.cc sunset_modem2phy-clmsg.h \
				initlib.cc

libSunset_Core_Modem_Phy_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -W -Wall
libSunset_Core_Modem_Phy_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -W -Wall -L../../Utilities/Sunset_Debug
libSunset_Core_Modem_Phy_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lpthread -lSunset_Core_Debug

nodist_libSunset_Core_Modem_Phy_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_modem2phy-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Modem_Phy_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
