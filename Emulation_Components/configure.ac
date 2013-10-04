AC_INIT(emulation_components,1.0)
AM_INIT_AUTOMAKE
AC_PROG_CXX
AC_PROG_MAKE_SET

AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_PATH_NS_ALLINONE
AC_ARG_WITH_NSMIRACLE
AC_CHECK_NSMIRACLE([have_nsmiracle=yes],[have_nsmiracle=no])
if test x$have_nsmiracle != xyes ; then
  AC_MSG_ERROR([Could not find nsmiracle,
    is --with-nsmiracle set correctly?])
fi
 
AC_ARG_WITH_SUNSET_CORE
AC_CHECK_SUNSET_CORE([have_sunset_core=yes],[have_sunset_core=no])
if test x$have_sunset_core != xyes ; then
  AC_MSG_ERROR([Could not find sunset_core,
    is --with-sunset_core set correctly?])
fi


SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Acoustic_Modems/Sunset_Evologics'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Acoustic_Modems/Sunset_Evologics/Sunset_Evologics_v1_6'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Acoustic_Modems/Sunset_Evologics/Sunset_Evologics_v1_4'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Acoustic_Modems/Sunset_Generic_Modem'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Acoustic_Modems/Sunset_Micro_Modem'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Scheduler/Sunset_RT_Scheduler'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Connections'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Connections/Serial'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Connections/TCP'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Connections/UDP'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Debug_Emulation'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Timing_Emulation'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Utilities_Emulation'

AC_SUBST(SUNSET_CPPFLAGS)
AC_SUBST(SUNSET_LDFLAGS)
AC_SUBST(SUNSET_LIBADD)
AC_SUBST(SUNSET_CORE_CPPFLAGS)


CPPFLAGS="$CPPFLAGS $SUNSET_CORE_CPPFLAGS $SUNSET_CPPFLAGS -ggdb"

CFLAGS="${CFLAGS=} -Wall -g -ggdb"

AC_DEFINE(CPP_NAMESPACE,std)


AC_CONFIG_FILES([
		Makefile
		Utilities/Sunset_Debug_Emulation/Makefile
		Acoustic_Modems/Sunset_Micro_Modem/Makefile
		Acoustic_Modems/Sunset_Evologics/Sunset_Evologics_v1_6/Makefile
		Acoustic_Modems/Sunset_Evologics/Sunset_Evologics_v1_4/Makefile
		m4/Makefile
		])
		
AC_OUTPUT
  
