#! /bin/sh

# For reasons I don't understand, the APT packages witty and witty-dev do not
# contain wttest, which is the library needed to do UT with wt.
# Seems nobody is doing Unit Testing ?
# So we have to download a ready-made build that we did ourselves.

set -o errexit

VERSION=3.3.2
SHA256="7bc59ab471d11d4cfe1e0b16ed68a199553947b07a31623be5316efcb4848b84  wt-3.3.2-bin-ubu-1204.7z"
FILE=wt-${VERSION}-bin-ubu-1204.7z
URL=http://freefr.dl.sourceforge.net/project/wutils/travis-ci/${FILE}
BASEDIR=wt-download
SRCDIR=wt-${VERSION}

echo
echo "*** Working directory: `pwd`"

mkdir -p ${BASEDIR}
echo
echo "*** Downloading binary wt from ${URL}"
curl ${URL} --output ${BASEDIR}/${FILE}

cd ${BASEDIR}
echo
echo "*** Checking shasum"
echo "${SHA256}" > sum
shasum -a 256 -c sum

echo
echo "*** Extracting binary wt"
7z x ${FILE}

echo
echo "*** wt extracted succesfully"
