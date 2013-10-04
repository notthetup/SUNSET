lib_LTLIBRARIES = libSunset_Emulation_Micro_Modem.la

libSunset_Emulation_Micro_Modem_la_SOURCES = sunset_micro_modem.cc sunset_micro_modem.h sunset_micro_modem_messages.cc sunset_micro_modem_messages.h \
				sunset_micro_modem_connection.cc sunset_micro_modem_connection.h initlib.cc \
				ext_include/conv.c ext_include/hash.c \
				ext_include/libnmea2.c ext_include/libphf.c \
				ext_include/nmeahash.c ext_include/nmeakeys.c \
				ext_include/rvidhash.c ext_include/rvidkeys.c \
				ext_include/scan.c ext_include/string.c \
				ext_include/umcfhash.c ext_include/umcfkeys.c
				

libSunset_Emulation_Micro_Modem_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@ -I./ -I./ext_include/
libSunset_Emulation_Micro_Modem_la_LDFLAGS =  @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@ -L${SUNSET_LIB_FOLDER}/lib/ -L../Sunset_Generic_Modem/.libs -L../../Utilities/Sunset_Connections/.libs
libSunset_Emulation_Micro_Modem_la_LIBADD =   @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lpthread -lSunset_Core_Debug -lSunset_Core_Utilities \
			-lSunset_Emulation_Generic_Modem -lSunset_Core_Common_Header -lSunset_Core_Information_Dispatcher \
			-lSunset_Core_Statistics -lSunset_Emulation_Connection -lSunset_Core_PktConverter

nodist_libSunset_Emulation_Micro_Modem_la_SOURCES = initTcl.cc
BUILT_SOURCES = initTcl.cc
CLEANFILES = initTcl.cc

TCL_FILES =  sunset_micro_modem-init.tcl

initTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Sunset_Micro_Modem_TclCode > initTcl.cc

EXTRA_DIST = $(TCL_FILES)
