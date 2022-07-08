BUILD_DIR=cmake-build64
BUILD_TYPE=Release
PROJECT_DIR=$(pwd)
DRIVER_BIN_DIR="$PROJECT_DIR/build/odbc/bin"

mkdir cmake-build64
cd cmake-build64
cmake ../src -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCODE_COVERAGE="OFF" -DBUILD_SHARED_LIBS="OFF" -DWITH_TESTS="ON" -DWITH_ODBC="ON"
make -j 4
cd ..
