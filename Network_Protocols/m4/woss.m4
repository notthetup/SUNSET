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




AC_DEFUN([AC_ARG_WITH_WOSS],[

WOSS_PATH=''
WOSS_CPPLAGS=''
WOSS_LDFLAGS=''
WOSS_LIBADD=''

AC_ARG_WITH([woss],
	[AS_HELP_STRING([--with-woss=<directory>],
			[use woss installation in <directory>])],
	[
		if test "x$withval" != "xno" ; then

   		     if test -d $withval ; then

   			WOSS_PATH="${withval}"

			for dir in  \ 
				woss \
				woss/woss_def \
				woss/woss_db \
				woss_phy \
				uwm
			do

			echo "considering dir \"$WOSS_PATH/$dir\""

			WOSS_CPPFLAGS="$WOSS_CPPFLAGS -I${WOSS_PATH}/${dir}"

			done

			for lib in \
				UwmStd \
				WOSS \
				WOSSPhy
			do
				WOSS_LIBADD="$WOSS_LIBADD -l${lib}"
			done	

			WOSS_DISTCHECK_CONFIGURE_FLAGS="--with-woss=$withval"
			AC_SUBST(WOSS_DISTCHECK_CONFIGURE_FLAGS)

   		     else	

			AC_MSG_ERROR([woss path $withval is not a directory])
	
		     fi

		fi

	])

AC_SUBST(WOSS_CPPFLAGS)

])


AC_DEFUN([AC_CHECK_WOSS],[

BACKUP_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $NS_CPPFLAGS $NSMIRACLE_CPPFLAGS $WOSS_CPPFLAGS"

AC_LANG_PUSH(C++)

AC_MSG_CHECKING([for woss headers])


AC_PREPROC_IFELSE(
	[AC_LANG_PROGRAM([[
		#include<res-reader.h>
		ResReader* r; 
		]],[[
		]]  )],
		[
		 AC_MSG_RESULT([yes])
		 found_woss=yes
		[$1]
		],
		[
		 AC_MSG_RESULT([no])
		 found_woss=no
		[$2]
		#AC_MSG_ERROR([could not find woss])
		])


AM_CONDITIONAL([HAVE_WOSS], [test x$found_woss = xyes])

# Restoring to the initial value
CPPFLAGS="$BACKUP_CPPFLAGS"

AC_LANG_POP(C++)

])


