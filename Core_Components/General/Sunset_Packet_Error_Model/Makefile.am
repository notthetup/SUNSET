lib_LTLIBRARIES = libSunset_Core_Packet_Error_Model.la

libSunset_Core_Packet_Error_Model_la_SOURCES = sunset_packet_error_model.cc sunset_packet_error_model.h \
				initlib.cc

libSunset_Core_Packet_Error_Model_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Core_Packet_Error_Model_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@  -L../../Utilities/Sunset_Debug -L../../Utilities/Sunset_Utilities -L../Sunset_Module
libSunset_Core_Packet_Error_Model_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@  -lSunset_Core_Debug -lSunset_Core_Utilities -lSunset_Core_Module

nodist_libSunset_Core_Packet_Error_Model_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_packet_error_model-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Packet_Error_Model_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)