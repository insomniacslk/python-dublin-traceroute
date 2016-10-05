#!/bin/bash
cd /tmp
git clone https://github.com/insomniacslk/dublin-traceroute.git
cd dublin-traceroute
if [[ "$TRAVIS_OS_NAME" == "linux" ]]
then
    .travis/install_dependencies_linux.sh
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]
then
    .travis/install_dependencies_osx.sh
    brew update
    brew install python
    brew install python3
fi
mkdir build
cd build
cmake ..
make
sudo make install
