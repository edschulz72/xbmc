#!/bin/bash

set -e

. ./setenv.sh

BUILD_DIR=$(cd $(dirname $0);pwd)
XBMCROOT=$(cd $BUILD_DIR/../..;pwd)
PLAYERCOREROOT=$(cd $XBMCROOT/..;pwd)/playercore
DVDNAVROOT=$(cd $XBMCROOT/..;pwd)/dvdnav_binary
BDJVMROOT=$(cd $XBMCROOT/..;pwd)/bdjvm_binary

if [ $# -eq 3 ]; then
export VERSION_CODE=$1
export VERSION_NAME=$2
export VERSION_TIME=$3
fi

echo 
echo "################################################################" 
echo XBMCROOT: $XBMCROOT
echo BUILD_DIR: $BUILD_DIR
echo SDKROOT: $SDKROOT
echo NDKROOT: $NDKROOT
echo TOOLCHAINROOT: $TOOLCHAINROOT
echo VMC_DEPENDS: $VMC_DEPENDS
echo TARBALLS: $TARBALLS
echo DISABLE_DEBUG: $DISABLE_DEBUG
echo VERSION_CODE: $VERSION_CODE
echo VERSION_NAME: $VERSION_NAME
echo VERSION_TIME: $VERSION_TIME
echo "################################################################" 
echo 


cd $XBMCROOT
#rm libxbmc.so
#if [ $DISABLE_DEBUG = "yes" ] ; then
#  make release -C tools/depends/target/xbmc
#else
#  make debug -C tools/depends/target/xbmc
#fi

echo
echo "################################################################" 
echo wget-playercore
echo "################################################################" 
echo 
make wget-playercore


#git checkout xbmc/cores/vdmplayer/libplayercore.h
#git checkout xbmc/cores/vdmplayer/libplayercore_iplayer.h
#git checkout xbmc/cores/vdmplayer/libplayercore_xsource.h
#git checkout xbmc/cores/vdmplayer/DllPlayercore.h
if [ -d $PLAYERCOREROOT ]; then
  echo 
  echo "################################################################" 
  echo build playercore on $PLAYERCOREROOT
  echo "################################################################" 
  echo 
  make playercore PLAYERCOREROOT="$PLAYERCOREROOT"
  cp $PLAYERCOREROOT/xbmc/xbmc/playercore/libplayercore.h xbmc/cores/vdmplayer/
  cp $PLAYERCOREROOT/xbmc/xbmc/playercore/libplayercore_iplayer.h xbmc/cores/vdmplayer/
  cp $PLAYERCOREROOT/xbmc/xbmc/playercore/libplayercore_xsource.h xbmc/cores/vdmplayer/
  cp $PLAYERCOREROOT/xbmc/xbmc/playercore/DllPlayercore.h xbmc/cores/vdmplayer/
  #git add xbmc/cores/vdmplayer/libplayercore.h
  #git add xbmc/cores/vdmplayer/libplayercore_iplayer.h
  #git add xbmc/cores/vdmplayer/libplayercore_xsource.h
  #git add xbmc/cores/vdmplayer/DllPlayercore.h
  #git commit -m "merge playercore header files"
  #git push origin
fi

if [ -d $DVDNAVROOT ]; then
  echo 
  echo "################################################################" 
  echo build libdvdnav on $DVDNAVROOT
  echo "################################################################" 
  echo 
  make libdvdnav DVDNAVROOT="$DVDNAVROOT"
fi

if [ -d $BDJVMROOT ]; then
  echo 
  echo "################################################################" 
  echo build libbdjvm on $BDJVMROOT
  echo "################################################################" 
  echo 
  make libbdjvm BDJVMROOT="$BDJVMROOT"
fi

if [ -f xbmc/vidonme/VDMVersionUpdate.o ] ; then
  echo
  echo "################################################################" 
  echo remove xbmc/vidonme/VDMVersionUpdate.o
  echo "################################################################" 
  echo
  rm xbmc/vidonme/VDMVersionUpdate.o
fi

echo
echo "################################################################" 
echo build libvmc.so
echo "################################################################" 
echo
make -j32
 
echo
echo "################################################################" 
echo build apk
echo "################################################################" 
echo
make apk



echo "################################################################" 
echo build finish!
echo "################################################################" 
