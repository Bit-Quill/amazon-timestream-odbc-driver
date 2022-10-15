#!/bin/bash

Usage="Usage: $0 <32|64> <DEBUG|RELEASE> <DEB|RPM>"
if [[ $# -ne 3 ]]
then
    echo "Invalid parameters"
    echo $Usage
    exit 1
fi

BUILD_DIR=cmake-build"$1"
PROJECT_DIR=$(pwd)
DRIVER_BIN_DIR="$PROJECT_DIR/build/odbc/bin"

if [[ $2=="DEBUG" ]]
then
    DRIVER_LOG_DIR="$PROJECT_DIR/build/odbc/logs"
    mkdir $DRIVER_LOG_DIR
fi

if [[ $1 -eq 32 ]]
then
    CMAKE_TOOLCHAIN_FILE="$PROJECT_DIR/src/linux_32bit_toolchain.cmake"
    
    # build AWS SDK
    cd src
    git clone --recurse-submodules -b "1.9.79" "https://github.com/aws/aws-sdk-cpp.git"
    cd aws-sdk-cpp
    mkdir install
    mkdir build
    cd build
    cmake ../ -DCMAKE_INSTALL_PREFIX="../install" -DCMAKE_BUILD_TYPE="Release" -DBUILD_ONLY="core;sts;timestream-query" -DCUSTOM_MEMORY_MANAGEMENT="OFF" -DENABLE_TESTING="OFF" -DBUILD_SHARED_LIBS="OFF" -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN}" -DCPP_STANDARD="17"
    make -j 4
    make install
    cd ../../../
elif [[ $1 -eq 64 ]]
then
    VCPKG_TARGET_TRIPLET="x64-linux"
    if [ -z "$VCPKG_ROOT" ]; then
       VCPKG_ROOT="$PROJECT_DIR/vcpkg"
    fi
    CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
else
    echo "Invalid parameter: $1"
    echo $Usage
    exit 1
fi

# build Timestream ODBC driver
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ../src -DCMAKE_BITNESS=$1 -DCMAKE_BUILD_TYPE=$2 -DCODE_COVERAGE="OFF" -DBUILD_SHARED_LIBS="OFF" -DWITH_TESTS="ON" -DWITH_ODBC="ON" -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN_FILE" -DINSTALLER_TYPE=$3 -DVCPKG_TARGET_TRIPLET="$VCPKG_TARGET_TRIPLET"
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
