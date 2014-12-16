#!/bin/bash

set -e

. ./setenv.sh


BUILD_DIR=$(cd $(dirname $0);pwd)
XBMCROOT=$(cd $BUILD_DIR/../..;pwd)
echo BUILD_DIR: $BUILD_DIR
echo XBMCROOT: $XBMCROOT
echo SDKROOT: $SDKROOT
echo NDKROOT: $NDKROOT
echo TOOLCHAINROOT: $TOOLCHAINROOT
echo VMC_DEPENDS: $VMC_DEPENDS
echo TARBALLS: $TARBALLS
echo DISABLE_DEBUG: $DISABLE_DEBUG

cd $XBMCROOT
#find . -name bootstrap -o -name configure -o -name configure2 -o -name Makefile.am -o -name "*.in" -o -name "*.sh"|xargs chmod a+x
#find . -name bootstrap -o -name configure -o -name configure2 -o -name Makefile.am -o -name "*.in"  -o -name "*.sh"|xargs fromdos

cd tools/depends

./bootstrap
./configure \
	--with-tarballs=$TARBALLS \
	--host=arm-linux-androideabi \
	--with-sdk-path=$SDKROOT \
	--with-ndk=$NDKROOT \
	--with-toolchain=$TOOLCHAINROOT \
	--prefix=$VMC_DEPENDS
make

cd $XBMCROOT
#rm libxbmc.so
if [ $DISABLE_DEBUG="yes" ] ; then
  make release -C tools/depends/target/xbmc
else
  make debug -C tools/depends/target/xbmc
fi


