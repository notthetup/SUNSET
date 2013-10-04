
lib_LTLIBRARIES = libSunset_Core_Module.la

libSunset_Core_Module_la_SOURCES = sunset_module.cc sunset_module.h \
				initlib.cc

libSunset_Core_Module_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -ggdb
libSunset_Core_Module_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L../../Utilities/Sunset_Debug -L../../Utilities/Sunset_Information_Dispatcher
libSunset_Core_Module_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lSunset_Core_Debug -lSunset_Core_Information_Dispatcher

nodist_libSunset_Core_Module_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_module-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Module_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
