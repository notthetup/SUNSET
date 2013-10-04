AC_INIT(core_components,1.0)
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
 

SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/General/Sunset_Common_Header'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/General/Sunset_Energy_Model'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/General/Sunset_Module'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/General/Sunset_Queue'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/General/Sunset_Timing'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/General/Sunset_Packet_Error_Model'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/ClMessage/Sunset_Modem2Phy'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/ClMessage/Sunset_Phy2Mac'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/ClMessage/Sunset_Mac2Rtg'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Packet_Converter/Sunset_Common_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Packet_Converter/Sunset_Ns_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Packet_Converter/Sunset_Pkt_Converter'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Statistics/Sunset_Statistics'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Debug'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Trace'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Information_Dispatcher'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Utilities'
SUNSET_CPPFLAGS="$SUNSET_CPPFLAGS "'-I$(top_srcdir)/Utilities/Sunset_Position'

AC_SUBST(SUNSET_CPPFLAGS)
AC_SUBST(SUNSET_LDFLAGS)
AC_SUBST(SUNSET_LIBADD)
AC_SUBST(WOSS_CPPFLAGS)


CPPFLAGS="$CPPFLAGS $SUNSET_CPPFLAGS $WOSS_CPPFLAGS -ggdb"

CFLAGS="${CFLAGS=} -Wall -g -ggdb"

AC_DEFINE(CPP_NAMESPACE,std)


AC_CONFIG_FILES([
		Makefile
		Utilities/Sunset_Debug/Makefile
		Utilities/Sunset_Trace/Makefile
		Utilities/Sunset_Utilities/Makefile
		Utilities/Sunset_Information_Dispatcher/Makefile
		Statistics/Sunset_Statistics/Makefile
		General/Sunset_Common_Header/Makefile
		General/Sunset_Module/Makefile
		General/Sunset_Energy_Model/Makefile
		General/Sunset_Queue/Makefile
		General/Sunset_Timing/Makefile
		General/Sunset_Packet_Error_Model/Makefile
		Utilities/Sunset_Position/Makefile
		ClMessage/Sunset_Modem2Phy/Makefile
		ClMessage/Sunset_Phy2Mac/Makefile
		ClMessage/Sunset_Mac2Rtg/Makefile
		m4/Makefile
		])
		
AC_OUTPUT
  

