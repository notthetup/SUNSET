#! /bin/bash

SUNSET_FOLDER=`pwd`
SUNSET_LIB_FOLDER=`pwd`/../build/sunset_lib
SUNSET_LIB_STRIPPED=`pwd`/../build/stripped/sunset_lib
NS_FOLDER=$SUNSET_FOLDER/../
NSMIRACLE_FOLDER=$SUNSET_FOLDER/../ns-miracle-1.0/trunk/main

mkdir -p $SUNSET_LIB_FOLDER

export SUNSET_FOLDER
export SUNSET_LIB_FOLDER
export WOSS_FOLDER

checkbashrc() {

	grep -e "#SUNSETv2 framework variable" ~/.bashrc > /dev/null

	if [ $? -eq 1 ] ; then
		echo "#SUNSETv2 framework variable" >> ~/.bashrc
		echo "" >> ~/.bashrc
	fi

	grep -e "SUNSET_LIB_FOLDER" ~/.bashrc > /dev/null

	if [ $? -eq 1 ] ; then
		echo "export SUNSET_LIB_FOLDER=$SUNSET_LIB_FOLDER" >> ~/.bashrc
	fi

	return
}

finish() {

	echo ""
	echo "*********************************************************************"
	echo "*                                                                   *"
	echo "*                                                                   *"
	echo "*    All the SUNSET Network_Protocols libraries have been           *"
	echo "*    installed in \"\$SUNSET_LIB_FOLDER/\"                             *"
	echo "*                                                                   *" 
	echo "*    IMPORTANT NOTICE:                                              *"
	echo "*                                                                   *"
	echo "*    Please run \"source ~/.bashrc\" or \". ~/.bashrc\"                 *"
	echo "*    to have the exported path (SUNSET_LIB_FOLDER), needed          *"
	echo "*    by SUNSET, available in the current bash environment.          *"
	echo "*                                                                   *"
	echo "*                                                                   *"
	echo "*********************************************************************"
	echo ""
	echo "     YOUR SUNSET_LIB_FOLDER is \"$SUNSET_LIB_FOLDER\""
	echo ""
	echo "*********************************************************************"

	return;
}

checkbashrc

cd  Network_Protocols/Addon/Packet_Converter

make distclean
rm -fr autom4te.cache/
rm config.*
rm missing 
rm Makefile.in 
rm install-sh 
rm depcomp 
rm configure
rm ltmain.sh 
rm m4/libtool*
rm m4/Makefile
rm m4/Makefile.in
rm aclocal.m4
rm m4/lt*

find . -name "*.o" -exec rm {} \;
find . -name "*.lo" -exec rm {} \;
find . -name "*.la" -exec rm {} \;
find . -name "*.libs" -exec rm -fr {} \;
find . -name "*.deps" -exec rm -fr {} \;
find . -name "Makefile" -exec rm -fr {} \;
find . -name "Makefile.in" -exec rm -fr {} \;

rm $SUNSET_LIB_FOLDER/lib/libSunset_Networking*PktConverter*
rm $SUNSET_LIB_STRIPPED/lib/libSunset_Networking*PktConverter*

./autogen.sh

./configure \
--prefix=$SUNSET_LIB_FOLDER \
--disable-static --enable-shared \
--with-ns-allinone=$NS_FOLDER \
--with-nsmiracle=$NSMIRACLE_FOLDER \
--with-sunset_core=$SUNSET_FOLDER/Core_Components \
--with-sunset_networking=$SUNSET_FOLDER/Network_Protocols

if [ $? -eq 0 ]; then
	echo "Operation correctly completed"
else
	echo "Error compiling SUNSET Network libraries EXIT"
	exit
fi

make

if [ $? -eq 0 ]; then
	echo "Operation correctly completed"
else
	echo "Error compiling SUNSET Network libraries EXIT"
	exit
fi

make install

if [ $? -eq 0 ]; then
	echo "Operation correctly completed"
else
	echo "Error compiling SUNSET Network libraries EXIT"
	exit
fi

mkdir -p $SUNSET_LIB_STRIPPED/lib/
cmd_target_strip='strip --strip-unneeded'

for f in "$SUNSET_LIB_FOLDER/lib/"libSunset_Networking*PktConverter*.so*; do
$cmd_target_strip "$f" -o ${SUNSET_LIB_STRIPPED}/lib/"$(basename "$f")"
done

finish

exit 0
