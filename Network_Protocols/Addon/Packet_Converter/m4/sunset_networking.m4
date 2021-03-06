#
# Copyright (c) 2007 Regents of the SIGNET lab, University of Padova.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University of Padova (SIGNET lab) nor the 
#    names of its contributors may be used to endorse or promote products 
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#




AC_DEFUN([AC_ARG_WITH_SUNSET_NETWORKING],[

SUNSET_NETWORKING_PATH=''
SUNSET_NETWORKING_CPPFLAGS=''
SUNSET_NETWORKING_LDFLAGS=''
SUNSET_NETWORKING_LIBADD=''

AC_ARG_WITH([sunset_networking],
	[AS_HELP_STRING([--with-sunset_networking=<directory>],
			[use sunset_networking installation in <directory>])],
	[
		if test "x$withval" != "xno" ; then

   		     if test -d $withval ; then

   			SUNSET_NETWORKING_PATH="${withval}"

	    		if test ! -f "${SUNSET_NETWORKING_PATH}/Application/Sunset_Agent/sunset_agent.h"  ; then
			  	AC_MSG_ERROR([could not find ${withval}/Application/Sunset_Agent/sunset_agent.h, 
  is --with-sunset_networking=${withval} correct?])
			fi		


			for dir in  \ 
				Application/Sunset_Agent \
				Application/Sunset_Agent/Sunset_Agent_Pkt \
				Datalink/Sunset_Mac \
				Datalink/Sunset_Mac/Sunset_Mac_Pkt \
				Datalink/Sunset_Aloha \
				Datalink/Sunset_Csma_Aloha \
				Datalink/Sunset_Csma \
				Datalink/Sunset_Csma_Localization \
				Datalink/Sunset_Csma_Localization/Sunset_Csma_Localization_Pkt \
				Datalink/Sunset_Dacap \
				Datalink/Sunset_SlottedCsma \
				Datalink/Sunset_Tdma \
				Datalink/Sunset_TLohi \
				Transport/Sunset_Transport \
				Network/Sunset_Routing \
				Network/Sunset_Routing/Sunset_Routing_Pkt \
				Network/Sunset_Static_Routing \
				Network/Sunset_Flooding \
				Phy/Sunset_Phy \
				Phy/Sunset_Phy_Uw/Sunset_Phy_Bellhop \
				Phy/Sunset_Phy_Uw/Sunset_Phy_Urick \
				Addon/Statistics/Sunset_Protocols_Statistics
	
			do

			echo "considering dir \"$dir\""

			SUNSET_NETWORKING_CPPFLAGS="$SUNSET_NETWORKING_CPPFLAGS -I${SUNSET_NETWORKING_PATH}/${dir}"
			SUNSET_NETWORKING_LDFLAGS="$SUNSET_NETWORKING_LDFLAGS -L${SUNSET_NETWORKING_PATH}/${dir}"

			done


			SUNSET_NETWORKING_DISTCHECK_CONFIGURE_FLAGS="--with-sunset_networking=$withval"
			AC_SUBST(SUNSET_NETWORKING_DISTCHECK_CONFIGURE_FLAGS)

   		     else	

			AC_MSG_ERROR([sunset_networking path $withval is not a directory])
	
		     fi

		fi

	])


AC_SUBST(SUNSET_NETWORKING_CPPFLAGS)
AC_SUBST(SUNSET_NETWORKING_LDFLAGS)

])



AC_DEFUN([AC_CHECK_SUNSET_NETWORKING],[

# if test "x$NS_CPPFLAGS" = x ; then
# 	true
# 	AC_MSG_ERROR([NS_CPPFLAGS is empty!])	
# fi

# if test "x$SUNSET_NETWORKING_CPPFLAGS" = x ; then
# 	true
# 	AC_MSG_ERROR([SUNSET_NETWORKING_CPPFLAGS is empty!])	
# fi	

# temporarily add NS_CPPFLAGS and SUNSET_NETWORKING_CPPFLAGS to CPPFLAGS
BACKUP_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $NS_CPPFLAGS $NSMIRACLE_CPPFLAGS $SUNSET_CORE_CPPFLAGS $SUNSET_NETWORKING_CPPFLAGS"

AC_LANG_PUSH(C++)

AC_MSG_CHECKING([for sunset_networking headers])


AC_PREPROC_IFELSE(
	[AC_LANG_PROGRAM([[
		#include <sunset_agent.h>
		Sunset_Agent* a;
		]],[[
		]]  )],
		[
		 AC_MSG_RESULT([yes])
		 found_sunset_networking=yes
		[$1]
		],
		[
		 AC_MSG_RESULT([no])
		 found_sunset_networking=no
		[$2]
		#AC_MSG_ERROR([could not find sunset_networking])
		])


AM_CONDITIONAL([HAVE_SUNSET_NETWORKING], [test x$found_sunset_networking = xyes])

# Restoring to the initial value
CPPFLAGS="$BACKUP_CPPFLAGS"

AC_LANG_POP(C++)

])

