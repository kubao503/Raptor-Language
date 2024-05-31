#!/bin/bash
BUILD_TYPE=Debug

apt-get -y install cmake
apt-get -y install lcov
apt-get -y install doxygen

curl -L https://github.com/conan-io/conan/releases/download/2.3.2/conan-2.3.2-amd64.deb > conan-2.3.2-amd64.deb
dpkg -i conan-2.3.2-amd64.deb

conan profile detect --force
conan install . --build=missing -s compiler.cppstd=20 -s build_type=$BUILD_TYPE

cd build/$BUILD_TYPE
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build .

./src/raptor_lang_interpreter ../../example.rp