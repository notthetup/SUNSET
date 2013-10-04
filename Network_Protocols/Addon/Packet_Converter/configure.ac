AC_INIT(network_protocols,1.0)
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

AC_ARG_WITH_SUNSET_NETWORKING
AC_CHECK_SUNSET_NETWORKING([have_sunset_networking=yes],[have_sunset_networking=no])
if test x$have_sunset_networking != xyes ; then
  AC_MSG_ERROR([Could not find sunset_networking,
    is --with-sunset_networking set correctly?])
fi

SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Application/Sunset_Agent_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Application/Sunset_Cbr_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_Mac_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Network/Sunset_Routing_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Phy/Sunset_Phy_Pkt_Converter'

AC_SUBST(SUNSET_CPPFLAGS)
AC_SUBST(SUNSET_LDFLAGS)
AC_SUBST(SUNSET_LIBADD)
AC_SUBST(SUNSET_CORE_CPPFLAGS)
AC_SUBST(WOSS_CPPFLAGS)


CPPFLAGS="$CPPFLAGS $SUNSET_CORE_CPPFLAGS $SUNSET_NETWORKING_CPPFLAGS $SUNSET_CPPFLAGS  -ggdb"

CFLAGS="${CFLAGS=} -Wall -g -ggdb"

AC_DEFINE(CPP_NAMESPACE,std)


AC_CONFIG_FILES([
		Makefile
		Application/Sunset_Agent_Pkt_Converter/Makefile
		Application/Sunset_Cbr_Pkt_Converter/Makefile
		Network/Sunset_Routing_Pkt_Converter/Makefile
		Datalink/Sunset_Mac_Pkt_Converter/Makefile
		Phy/Sunset_Phy_Pkt_Converter/Makefile
		m4/Makefile
		])
		
AC_OUTPUT
  
