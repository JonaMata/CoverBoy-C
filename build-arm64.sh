cp -r /work ~/work
cd ~/work
sudo apt update
sudo apt install -y devscripts debhelper
sed -i 's/-march=native/-march=armv8-a/g' rpi-rgb-led-matrix/lib/Makefile
make -C rpi-rgb-led-matrix/lib
conan install . --build=missing --profile:build=build-profile --profile:host=host-profile-arm64
source build/Release/generators/conanbuild.sh
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
debuild --prepend-path=/usr/xcc/aarch64-unknown-linux-gnu/bin -us -uc -b -aarm64 -taarch64-unknown-linux-gnu --host-arch arm64 --host-type aarch64-unknown-linux-gnu
mkdir -p /work/build
cp ../*.deb /work/build/
cp debian/coverboy/usr/bin/coverboy /work/build/coverboy-arm64
