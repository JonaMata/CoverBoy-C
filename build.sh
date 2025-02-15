cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
debuild -uc -us -b
mkdir deb
cp ../coverboy*.deb deb/