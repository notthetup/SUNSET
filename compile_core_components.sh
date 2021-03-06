#! /bin/bash

SUNSET_FOLDER=`pwd`
SUNSET_LIB_FOLDER=`pwd`/../build/sunset_lib
SUNSET_LIB_STRIPPED=`pwd`/../build/stripped/sunset_lib
NS_FOLDER=$SUNSET_FOLDER/../
NSMIRACLE_FOLDER=$SUNSET_FOLDER/../ns-miracle-1.0/trunk/main

mkdir -p $SUNSET_LIB_FOLDER

export SUNSET_FOLDER
export SUNSET_LIB_FOLDER

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
	echo "*    All the SUNSET Core_Components libraries have been             *"
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

cd Core_Components

rm $SUNSET_LIB_FOLDER/lib/libSunset_Core*
rm $SUNSET_LIB_STRIPPED/lib/libSunset_Core*


make

if [ $? -eq 0 ]; then
	echo "Operation correctly completed"
else
	echo "Error compiling SUNSET Core libraries EXIT"
	exit
fi

make install

if [ $? -eq 0 ]; then
	echo "Operation correctly completed"
else
	echo "Error compiling SUNSET Core libraries EXIT"
	exit
fi

for f in `find ./Packet_Converter -name "*PktConverter*.so"`; do
cp "$f" $SUNSET_LIB_FOLDER/lib/
done

for f in `find ./Packet_Converter -name "*PktConverter*.so*.0"` ; do
cp "$f" $SUNSET_LIB_FOLDER/lib/
done


mkdir -p $SUNSET_LIB_STRIPPED/lib/
cmd_target_strip='strip --strip-unneeded'

for f in "$SUNSET_LIB_FOLDER/lib/"libSunset_Core*.so*; do
$cmd_target_strip "$f" -o ${SUNSET_LIB_STRIPPED}/lib/"$(basename "$f")"
done

finish

exit 0
