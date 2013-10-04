
lib_LTLIBRARIES = libSunset_Core_Common_Header.la

libSunset_Core_Common_Header_la_SOURCES = 	sunset_common_pkt.cc \
				sunset_common_pkt.h \
				initlib.cc

libSunset_Core_Common_Header_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Common_Header_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../../Utilities/Sunset_Debug
libSunset_Core_Common_Header_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug

nodist_libSunset_Core_Common_Header_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_common_pkt-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Common_Header_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
