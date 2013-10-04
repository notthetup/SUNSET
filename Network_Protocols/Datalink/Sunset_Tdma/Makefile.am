lib_LTLIBRARIES = libSunset_Networking_Tdma.la

libSunset_Networking_Tdma_la_SOURCES = sunset_tdma.cc sunset_tdma.h \
				sunset_tdma_timers.cc sunset_tdma_timers.h \
				initlib.cc

libSunset_Networking_Tdma_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Networking_Tdma_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -L${SUNSET_LIB_FOLDER}/lib/  -L../Sunset_Mac
libSunset_Networking_Tdma_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@  -lSunset_Core_Mac_Routing \
			 -lSunset_Core_Phy_Mac -lSunset_Core_Queue -lSunset_Core_Utilities -lSunset_Core_Statistics \
			-lSunset_Core_Timing  -lSunset_Core_Information_Dispatcher -lSunset_Core_Debug \
			-lSunset_Networking_Mac -lSunset_Core_Module -lSunset_Core_Trace -lSunset_Core_Common_Header

nodist_libSunset_Networking_Tdma_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_tdma-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Tdma_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
