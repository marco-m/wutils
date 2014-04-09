#! /bin/sh

set -o errexit

VERSION=1.7.0
SHA256="26fcbb5925b74ad5fc8c26b0495dfc96353f4d553492eb97e85a8a6d2f43095b  gmock-1.7.0.zip"
FILE=gmock-${VERSION}.zip
URL=https://googlemock.googlecode.com/files/${FILE}
BASEDIR=gmock-download
SRCDIR=gmock-${VERSION}
BIN_ARCHIVE=gmock-${VERSION}-bin-ubu-1204.7z

echo
echo "*** Working directory: `pwd`"

mkdir -p ${BASEDIR}
echo
echo "*** Downloading gmock/gtest"
curl ${URL} --output ${BASEDIR}/${FILE}

cd ${BASEDIR}
# Also if https, we check the shasum
echo
echo "*** Checking shasum"
echo "${SHA256}" > sum
shasum -a 256 -c sum

echo
echo "*** Extracting gmock/gtest"
unzip -q ${FILE}

mkdir -p build
cd build
echo
echo "*** Configuring gmock/gest"
export CXX=clang++3.4
cmake -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++" cmake ../${SRCDIR}

echo
echo "*** Building gmock/gtest"
make

echo
echo "*** Installing gmock/gtest"
cd ..
mkdir -p install/lib
mkdir -p install/include
cp build/libgmock* install/lib/
cp build/gtest/libgtest* install/lib/
cp -R ${SRCDIR}/include/gmock install/include/
cp -R ${SRCDIR}/gtest/include/gtest install/include/

echo
echo "*** gmock/gtest built and installed succesfully"

echo
echo "*** Creating archive"
7z a ${BIN_ARCHIVE} install

echo
echo "*** SHA256:"
shasum -a 256 ${BIN_ARCHIVE}

