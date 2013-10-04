
lib_LTLIBRARIES = libSunset_Networking_Agent.la

libSunset_Networking_Agent_la_SOURCES = sunset_agent.cc sunset_agent.h \
				Sunset_Agent_Pkt/sunset_agent_pkt.h \
				Sunset_Agent_Pkt/sunset_agent_pkt.cc \
				initlib.cc

libSunset_Networking_Agent_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Networking_Agent_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/
libSunset_Networking_Agent_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lSunset_Core_Trace -lSunset_Core_Utilities \
			-lSunset_Core_Statistics -lSunset_Core_Timing -lSunset_Core_Information_Dispatcher -lSunset_Core_Module  -lSunset_Core_Common_Header

nodist_libSunset_Networking_Agent_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_agent-init.tcl Sunset_Agent_Pkt/sunset_agt_pkt-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Agent_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
