BUILD_DIR=cmake-build64
BUILD_TYPE=Release
PROJECT_DIR=$(pwd)
DRIVER_BIN_DIR="$PROJECT_DIR/build/odbc/bin"
VCPKG_TARGET_TRIPLET="x64-linux"

if [ -z "$VCPKG_ROOT" ]; then
   VCPKG_ROOT="$PROJECT_DIR/vcpkg"
fi

CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

mkdir cmake-build64
cd cmake-build64
cmake ../src -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCODE_COVERAGE="OFF" -DBUILD_SHARED_LIBS="OFF" -DWITH_TESTS="ON" -DWITH_ODBC="ON" -DVCPKG_TARGET_TRIPLET="$VCPKG_TARGET_TRIPLET" -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN_FILE"
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
