
lib_LTLIBRARIES = libSunset_Networking_Mac.la

libSunset_Networking_Mac_la_SOURCES = Sunset_Mac_Pkt/sunset_mac_pkt.cc \
				Sunset_Mac_Pkt/sunset_mac_pkt.h \
				sunset_mac.cc sunset_mac.h \
				sunset_mac_timers.cc sunset_mac_timers.h \
				initlib.cc

libSunset_Networking_Mac_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Networking_Mac_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ 
libSunset_Networking_Mac_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@  -lSunset_Core_Mac_Routing \
			 -lSunset_Core_Phy_Mac -lSunset_Core_Queue -lSunset_Core_Utilities -lSunset_Core_Statistics \
			-lSunset_Core_Timing  -lSunset_Core_Information_Dispatcher -lSunset_Core_Debug \
			-lSunset_Core_Module -lSunset_Core_Trace 

nodist_libSunset_Networking_Mac_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_mac-init.tcl Sunset_Mac_Pkt/sunset_mac_pkt-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Mac_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
