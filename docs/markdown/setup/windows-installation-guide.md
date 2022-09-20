# Amazon Timestream ODBC Driver Setup on Windows

## Install Timestream ODBC Driver
1. Download the Timestream ODBC installer exe.
2. Double-click the installer
3. Follow the instructions and finish the installation.

## Notes
Microsoft Visual C++ Redistributable Package will be installed automatically if you do not have already installed in your System.

For dependency uninstallation after Timestream ODBC driver is uninstalled, a window for Microsoft Visual C++ Redistributable Package
Modify Setup could pop up with three options, "repair", "uninstall" and "close". If you choose "repair" the Microsoft Visual C++ 
Redistributable Package will be repaired or reinstalled and the Timestream ODBC driver will be removed. The "uninstall" will uninstall 
the Microsoft Visual C++ Redistributable Package. If you choose "close" the Timestream ODBC driver uninstallation will be rolled back. 

## Next Steps

- [Set up the DSN](dsn-configuration.md)
