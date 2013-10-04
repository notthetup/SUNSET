
lib_LTLIBRARIES = libSunset_Emulation_Debug_Emulation.la

libSunset_Emulation_Debug_Emulation_la_SOURCES = sunset_debug_emulation.cc sunset_debug_emulation.h \
				 initlib.cc

libSunset_Emulation_Debug_Emulation_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Emulation_Debug_Emulation_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/
libSunset_Emulation_Debug_Emulation_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lrt -lSunset_Core_Debug -lpthread

nodist_libSunset_Emulation_Debug_Emulation_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_debug_emulation-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Debug_Emulation_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
