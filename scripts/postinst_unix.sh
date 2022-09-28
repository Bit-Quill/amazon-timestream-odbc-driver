#!/bin/bash

ODBC_LIB_PATH="/usr/lib64/timestream-odbc64"
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

ODBC_INSTALL_INI="$ODBC_LIB_PATH/timestream-odbc-install.ini"

echo "[Amazon Timestream ODBC Driver]"            > $ODBC_INSTALL_INI
echo "Description=Amazon Timestream ODBC Driver" >> $ODBC_INSTALL_INI
echo "Driver=$ODBC_LIB_FILENAME" >> $ODBC_INSTALL_INI
echo "Setup=$ODBC_LIB_FILENAME"  >> $ODBC_INSTALL_INI
echo "DriverODBCVer=03.00"       >> $ODBC_INSTALL_INI
echo "FileUsage=0"               >> $ODBC_INSTALL_INI

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  odbcinst -i -d -f $ODBC_INSTALL_INI
elif [[ "$OSTYPE" == "darwin"* ]]; then
  export ODBCINSTINI=$ODBC_INSTALL_INI
  echo "Exported ODBCINSTINI=$ODBCINSTINI"
  export DYLD_LIBRARY_PATH=$ODBC_LIB_PATH:$DYLD_LIBRARY_PATH
  echo "Exported DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
fi
