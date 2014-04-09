#! /bin/sh

set -o errexit

VERSION=1.7.0
SHA256="dad994e39d898333cd3e236e34e05ad6c7c37f8d3d1718d5e105c47c32735c29  gmock-1.7.0-bin-ubu-1204.7z"
FILE=gmock-${VERSION}-bin-ubu-1204.7z
URL=http://freefr.dl.sourceforge.net/project/wutils/travis-ci/${FILE}
BASEDIR=gmock-download
SRCDIR=gmock-${VERSION}

echo
echo "*** Working directory: `pwd`"

mkdir -p ${BASEDIR}
echo
echo "*** Downloading binary gmock/gtest from ${URL}"
curl ${URL} --output ${BASEDIR}/${FILE}

cd ${BASEDIR}
# Also if https, we check the shasum
echo
echo "*** Checking shasum"
echo "${SHA256}" > sum
shasum -a 256 -c sum

echo
echo "*** Extracting binary gmock/gtest"
7z x ${FILE}

echo
echo "*** gmock/gtest extracted succesfully"
