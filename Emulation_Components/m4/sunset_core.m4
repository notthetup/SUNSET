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




AC_DEFUN([AC_ARG_WITH_SUNSET_CORE],[

SUNSET_CORE_PATH=''
SUNSET_CORE_CPPFLAGS=''
SUNSET_CORE_LDFLAGS=''
SUNSET_CORE_LIBADD=''

AC_ARG_WITH([sunset_core],
	[AS_HELP_STRING([--with-sunset_core=<directory>],
			[use sunset_core installation in <directory>])],
	[
		if test "x$withval" != "xno" ; then

   		     if test -d $withval ; then

   			SUNSET_CORE_PATH="${withval}"

	    		if test ! -f "${SUNSET_CORE_PATH}/Utilities/Sunset_Utilities/sunset_address.h"  ; then
			  	AC_MSG_ERROR([could not find ${withval}/Utilities/Sunset_Utilities/sunset_address.h, 
  is --with-sunset_core=${withval} correct?])
			fi		


			for dir in  \ 
				Utilities/Sunset_Debug \
				Utilities/Sunset_Trace \
				Utilities/Sunset_Utilities \
				Utilities/Sunset_Information_Dispatcher \
				Utilities/Sunset_Position \
				Statistics/Sunset_Statistics \
				General/Sunset_Common_Header \
				General/Sunset_Energy_Model \
				General/Sunset_Module \
				General/Sunset_Queue \
				General/Sunset_Timing \
				General/Sunset_Packet_Error_Model \
				ClMessage/Sunset_Modem2Phy \
				ClMessage/Sunset_Phy2Mac \
				ClMessage/Sunset_Mac2Rtg \
				Packet_Converter/Sunset_Pkt_Converter \
				Packet_Converter/Sunset_Common_Pkt_Converter \
				Packet_Converter/Sunset_Ns_Pkt_Converter
	
			do

			echo "considering dir \"$dir\""

			SUNSET_CORE_CPPFLAGS="$SUNSET_CORE_CPPFLAGS -I${SUNSET_CORE_PATH}/${dir}"
			SUNSET_CORE_LDFLAGS="$SUNSET_CORE_LDFLAGS -L${SUNSET_CORE_PATH}/${dir}"

			done

			for lib in \
				Utilities/Sunset_Debug \
				Utilities/Sunset_Utilities \
				Utilities/Sunset_Information_Dispatcher \
				Utilities/Sunset_Position \
				Statistics/Sunset_Statistics \
				General/Sunset_Common_Header \
				General/Sunset_Energy_Model \
				General/Sunset_Module \
				General/Sunset_Queue \
				General/Sunset_Timing \
				General/Sunset_Packet_Error_Model \
				ClMessage/Sunset_Modem2Phy \
				ClMessage/Sunset_Phy2Mac \
				ClMessage/Sunset_Mac2Rtg \
				Packet_Converter/Sunset_Pkt_Converter \
				Packet_Converter/Sunset_Common_Pkt_Converter \
				Packet_Converter/Sunset_Ns_Pkt_Converter
		
			do
				SUNSET_CORE_LIBADD="$SUNSET_CORE_LIBADD -l${lib}"
			done	



			SUNSET_CORE_DISTCHECK_CONFIGURE_FLAGS="--with-sunset_core=$withval"
			AC_SUBST(SUNSET_CORE_DISTCHECK_CONFIGURE_FLAGS)

   		     else	

			AC_MSG_ERROR([sunset_core path $withval is not a directory])
	
		     fi

		fi

	])


AC_SUBST(SUNSET_CORE_CPPFLAGS)
AC_SUBST(SUNSET_CORE_LDFLAGS)
AC_SUBST(SUNSET_CORE_LIBADD)

])



AC_DEFUN([AC_CHECK_SUNSET_CORE],[

# if test "x$NS_CPPFLAGS" = x ; then
# 	true
# 	AC_MSG_ERROR([NS_CPPFLAGS is empty!])	
# fi

# if test "x$SUNSET_CORE_CPPFLAGS" = x ; then
# 	true
# 	AC_MSG_ERROR([SUNSET_CORE_CPPFLAGS is empty!])	
# fi	

# temporarily add NS_CPPFLAGS and SUNSET_CORE_CPPFLAGS to CPPFLAGS
BACKUP_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $NS_CPPFLAGS $NSMIRACLE_CPPFLAGS $SUNSET_CORE_CPPFLAGS"


AC_LANG_PUSH(C++)

AC_MSG_CHECKING([for sunset_core headers])


AC_PREPROC_IFELSE(
	[AC_LANG_PROGRAM([[
		#include<sunset_debug.h>
		Sunset_Debug* t; 
		]],[[
		]]  )],
		[
		 AC_MSG_RESULT([yes])
		 found_sunset_core=yes
		[$1]
		],
		[
		 AC_MSG_RESULT([no])
		 found_sunset_core=no
		[$2]
		#AC_MSG_ERROR([could not find sunset_core])
		])


AM_CONDITIONAL([HAVE_SUNSET_CORE], [test x$found_sunset_core = xyes])

# Restoring to the initial value
CPPFLAGS="$BACKUP_CPPFLAGS"

AC_LANG_POP(C++)

])

