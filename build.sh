cp -r /work ~/work
cd ~/work
sudo apt update
sudo apt install -y devscripts debhelper
sed -i 's/-march=native/-march=armv6/g' rpi-rgb-led-matrix/lib/Makefile
make -C rpi-rgb-led-matrix/lib
conan install . --build=missing --profile:build=build-profile --profile:host=host-profile
source build/Release/generators/conanbuild.sh
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
debuild --prepend-path=/usr/xcc/armv6-unknown-linux-gnueabihf/bin -us -uc -b -aarmhf -tarmv6-unknown-linux-gnueabihf --host-arch armhf --host-type armv6-unknown-linux-gnueabihf
mkdir -p /work/build
cp ../*.deb /work/build/
cp debian/coverboy/usr/bin/coverboy /work/build/
