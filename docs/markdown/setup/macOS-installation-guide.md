# Amazon Timestream ODBC Driver Setup on MacOS

## Install Timestream ODBC Driver
1. Download the Timestream ODBC driver installer AmazonTimestreamODBC-[version].pkg.
2. Double-click the installer to install by GUI or use the following command line to install it directly.
```
sudo installer -pkg AmazonTimestreamODBC-[version].pkg -target /
```
3. Follow the instructions and finish the installation if you install by GUI.

## Next Steps

- [Set up the DSN](macOS-dsn-configuration.md)

## Uninstall Timestream ODBC Driver
There is no automatical way to uninstall it. It has to be done manually.
1. Remove "Amazon Timestream ODBC Driver" entry from /Library/ODBC/odbcinst.ini
2. Remove the driver residence direcotry /Library/ODBC/timestream-odbc
```
sudo rm -rf /Library/ODBC/timestream-odbc
```
