. envsetupIsengard.sh

filepath=$(cd "$(dirname "$0")"; pwd)
DEPENDS=$filepath/../Isengard-depends

git clean -dxf
#git submodule update --init addons/skin.re-touched
cd tools/depends
./bootstrap

./configure --with-toolchain=$TOOLCHAIN --prefix=$DEPENDS --host=arm-linux-androideabi --with-sdk-path=$SDK --with-ndk=$NDK --with-sdk=android-17 --with-tarballs=$TARBALLS

make -C target/android-sources-ics/ clean
make -C target/xbmc-audioencoder-addons/ clean
make -C target/librtmp/ clean

make -j8 -C target/binary-addons
make -C -j8  native/
make -C  native/
make -j8
cd ../../

make -C tools/depends/target/android-sources-ics/
make -C tools/depends/target/xbmc
make -j8
make apk

