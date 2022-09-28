#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

SCRIPT_DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

PROJECT_DIR="$SCRIPT_DIR/.."
ODBC_LIB_PATH="$PROJECT_DIR/build/odbc/lib"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  ODBC_LIB_FILENAME="$ODBC_LIB_PATH/libtimestream-odbc.so"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  ODBC_LIB_FILENAME="$ODBC_LIB_PATH/libtimestream-odbc.dylib"
fi


if [ ! -f "$ODBC_LIB_FILENAME" ]
then
  echo "Cannot find ODBC library file: $ODBC_LIB_FILENAME"
  exit 1
fi

echo "[Amazon Timestream ODBC Driver]"            > "$ODBC_LIB_PATH/timestream-odbc-install.ini"
echo "Description=Amazon Timestream ODBC Driver" >> "$ODBC_LIB_PATH/timestream-odbc-install.ini"
echo "Driver=$ODBC_LIB_FILENAME" >> "$ODBC_LIB_PATH/timestream-odbc-install.ini"
echo "Setup=$ODBC_LIB_FILENAME"  >> "$ODBC_LIB_PATH/timestream-odbc-install.ini"
echo "DriverODBCVer=03.00"       >> "$ODBC_LIB_PATH/timestream-odbc-install.ini"
echo "FileUsage=0"               >> "$ODBC_LIB_PATH/timestream-odbc-install.ini"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  odbcinst -i -d -f "$ODBC_LIB_PATH/timestream-odbc-install.ini"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  export ODBCINSTINI="$ODBC_LIB_PATH/timestream-odbc-install.ini"
  echo "Exported ODBCINSTINI=$ODBCINSTINI"
  export DYLD_LIBRARY_PATH=$ODBC_LIB_PATH:$DYLD_LIBRARY_PATH
  echo "Exported DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
fi
