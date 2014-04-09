#! /bin/sh

# For reasons I don't understand, the APT packages witty and witty-dev do not
# contain wttest, which is the library needed to do UT with wt.
# Seems nobody is doing UT...
# So we have to download and build ourselves.

set -o errexit

VERSION=3.3.2
SHA256="4c782f1ac335b770bc5b26f89814645264c7e6da32d56454707ab4acabaad827  wt-3.3.2.tar.gz"
FILE=wt-${VERSION}.tar.gz
URL=http://freefr.dl.sourceforge.net/project/witty/wt/${VERSION}/${FILE}
BASEDIR=wt-download
SRCDIR=wt-${VERSION}
BIN_ARCHIVE=wt-${VERSION}-bin-ubu-1204.7z

echo "*** Working directory: `pwd`"

mkdir -p ${BASEDIR}
echo "*** Downloading wt from ${URL}"
curl ${URL} --output ${BASEDIR}/${FILE}

cd ${BASEDIR}
echo "*** Checking shasum"
echo "${SHA256}" > sum
shasum -a 256 -c sum

echo "*** Extracting wt"
tar xzf ${FILE}

mkdir -p build
mkdir -p install
cd build
echo "*** Configuring wt"
# Boost on travis is so old we need also the NO_BOOST_RANDOM option.
#    -DWT_NO_BOOST_RANDOM=ON \
cmake \
    -DBUILD_EXAMPLES=OFF \
    -DENABLE_HARU=OFF \
    -DENABLE_PANGO=OFF \
    -DENABLE_EXT=OFF \
    -DENABLE_POSTGRES=OFF \
    -DENABLE_FIREBIRD=OFF \
    -DENABLE_MYSQL=OFF \
    -DENABLE_QT4=OFF \
    -DENABLE_OPENGL=OFF \
    -DBUILD_TESTS=OFF \
    -DCONNECTOR_FCGI=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=../install \
    -DCONFIGDIR=../install \
    -DCONFIGURATION=../install/wt_config.xml \
    -DWTHTTP_CONFIGURATION=../install/wthttpd \
    ../${SRCDIR}

echo "*** Building wt"
make

echo "*** Installing wt"
make install

echo "*** wt built and installed succesfully"

echo "*** Creating archive"
cd ..
7z a ${BIN_ARCHIVE} install

echo "*** SHA256:"
shasum -a 256 ${BIN_ARCHIVE}




