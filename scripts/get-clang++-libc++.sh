#!/bin/sh

# libc++ built following http://libcxx.llvm.org/
# section Build on Linux using CMake and libsupc++

set -o errexit

VERSION=3.4
SHA256="9a2e08d134287ae5f9b58a66005deb82089554e58a7addf5a4917bcb6c1986e2  clang++-3.4-libc++-bin-ubu1204.7z"
FILE=clang++-${VERSION}-libc++-bin-ubu1204.7z
URL=http://sourceforge.net/projects/wutils/files/travis-ci/${FILE}/download
BASEDIR=libc++-download


echo
echo "*** Working directory: `pwd`"

mkdir -p ${BASEDIR}
echo
echo "*** Downloading binary libc++ from ${URL}"
curl --location ${URL} --output ${BASEDIR}/${FILE}

cd ${BASEDIR}
echo
echo "*** Checking shasum"
echo "${SHA256}" > sum
shasum -a 256 -c sum

echo
echo "*** Extracting binary libc++"
DIR=`pwd`
cd /usr
echo "*** Working directory: `pwd`"
sudo 7z -y x ${DIR}/${FILE}

echo
echo "*** libc++ extracted succesfully"



