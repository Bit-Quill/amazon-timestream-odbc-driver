#!/bin/bash

# command format
# ./script_file_name <DEBUG|RELEASE> <DEB|RPM>
BUILD_DIR=cmake-build64
PROJECT_DIR=$(pwd)
DRIVER_BIN_DIR="$PROJECT_DIR/build/odbc/bin"
VCPKG_TARGET_TRIPLET="x64-linux"

if [ -z "$VCPKG_ROOT" ]; then
   VCPKG_ROOT="$PROJECT_DIR/vcpkg"
fi

CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

if [[ $1=="DEBUG" ]]
then
    DRIVER_LOG_DIR="$PROJECT_DIR/build/odbc/logs"
    mkdir $DRIVER_LOG_DIR
fi

mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ../src -DCMAKE_BUILD_TYPE=$1 -DCODE_COVERAGE="ON" -DBUILD_SHARED_LIBS="OFF" -DWITH_TESTS="ON" -DWITH_ODBC="ON" -DVCPKG_TARGET_TRIPLET="$VCPKG_TARGET_TRIPLET" -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN_FILE" -DINSTALLER_TYPE=$2
make -j 4

RET_CODE=$?

if [ $RET_CODE -ne 0 ]; then
   echo "Error occurred while building project. Exiting."
   exit $RET_CODE
fi

make package

RET_CODE=$?

if [ $RET_CODE -ne 0 ]; then
   echo "Error occurred while building package. Exiting."
   exit $RET_CODE
fi

cp *.deb $DRIVER_BIN_DIR
cd ..
