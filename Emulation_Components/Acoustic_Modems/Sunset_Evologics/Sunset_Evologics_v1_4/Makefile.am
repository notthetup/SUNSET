
lib_LTLIBRARIES = libSunset_Emulation_Evologics_v_one_four.la

libSunset_Emulation_Evologics_v_one_four_la_SOURCES =  sunset_evologics_v1_4.cc sunset_evologicsv1_4.h \
				../sunset_evologics_connection.cc ../sunset_evologics_connection.h	\
				../sunset_evologics_def.h ../sunset_evologics_include.h \
				initlib.cc

libSunset_Emulation_Evologics_v_one_four_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libSunset_Emulation_Evologics_v_one_four_la_LDFLAGS =  @NS_LDFLAGS@  @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ -L../../Sunset_Generic_Modem/.libs -L../../../Utilities/Sunset_Connections/.libs

libSunset_Emulation_Evologics_v_one_four_la_LIBADD =   @NS_LIBADD@  @NSMIRACLE_LIBADD@ -lpthread -lSunset_Core_Debug -lSunset_Core_Utilities -lSunset_Core_Common_Header -lSunset_Core_Statistics -lSunset_Core_PktConverter -lSunset_Emulation_Generic_Modem   -lSunset_Emulation_Connection 

nodist_libSunset_Emulation_Evologics_v_one_four_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_evologics_v1_4-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Evologics_v_one_four_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
