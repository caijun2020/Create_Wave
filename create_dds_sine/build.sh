#!/bin/sh

set -e
CURRENT_DIR=`pwd`

echo
echo "Building ......"
echo

cp -rf ../${CURRENT_DIR##*/} /home/vsp/test_cai/

cd /home/vsp/test_cai/${CURRENT_DIR##*/}

qmake -project
qmake
make distclean
make

cp ${CURRENT_DIR##*/} $CURRENT_DIR

echo "Building done."


