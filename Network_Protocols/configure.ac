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

AC_ARG_WITH_WOSS
AC_CHECK_WOSS([have_woss=yes],[have_woss=no])
if test x$have_woss != xyes ; then
  AC_MSG_WARN([Could not find woss -- The SUNSET Urick/WOSS will NOT be built])
fi


SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Addon/Statistics/Sunset_Protocols_Statistics'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Application/Sunset_Agent'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Application/Sunset_Agent/Sunset_Agent_Pkt'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_Mac'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_Mac/Sunset_Mac_Pkt'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_Aloha'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_Csma_Aloha'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_SlottedCsma'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Datalink/Sunset_Tdma'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Transport/Sunset_Transport'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Network/Sunset_Routing'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Network/Sunset_Routing/Sunset_Routing_Pkt'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Network/Sunset_Static_Routing'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Network/Sunset_Flooding'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Phy/Sunset_Phy'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Phy/Sunset_Phy_Uw/Sunset_Phy_Bellhop'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Phy/Sunset_Phy_Uw/Sunset_Phy_Urick'

AC_SUBST(SUNSET_CPPFLAGS)
AC_SUBST(SUNSET_LDFLAGS)
AC_SUBST(SUNSET_LIBADD)
AC_SUBST(SUNSET_CORE_CPPFLAGS)
AC_SUBST(WOSS_CPPFLAGS)


CPPFLAGS="$CPPFLAGS $SUNSET_CORE_CPPFLAGS $SUNSET_CPPFLAGS $WOSS_CPPFLAGS -ggdb"

CFLAGS="${CFLAGS=} -Wall -g -ggdb"

AC_DEFINE(CPP_NAMESPACE,std)


AC_CONFIG_FILES([
		Makefile
		Application/Sunset_Agent/Makefile
		Datalink/Sunset_Mac/Makefile
		Datalink/Sunset_Aloha/Makefile
		Datalink/Sunset_Csma_Aloha/Makefile
		Datalink/Sunset_SlottedCsma/Makefile
		Datalink/Sunset_Tdma/Makefile
		Transport/Sunset_Transport/Makefile
		Network/Sunset_Routing/Makefile
		Network/Sunset_Static_Routing/Makefile
		Network/Sunset_Flooding/Makefile
		Phy/Sunset_Phy/Makefile
		Phy/Sunset_Phy_Uw/Sunset_Phy_Bellhop/Makefile
		Phy/Sunset_Phy_Uw/Sunset_Phy_Urick/Makefile
		Addon/Statistics/Sunset_Protocols_Statistics/Makefile
		m4/Makefile
		])
		
AC_OUTPUT
  
