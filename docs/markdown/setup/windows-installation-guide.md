# Amazon Timestream ODBC Driver Setup on Windows

## Supported versions
Windows 10 22H2, Windows 11 and Windows Server 2022

## Install Timestream ODBC Driver
1. Download the Timestream ODBC installer exe.
2. Double-click the installer
3. Follow the instructions and finish the installation.

## Notes
### Microsoft Visual C++ Redistributable (MSVC)
Microsoft Visual C++ Redistributable (MSVC) Package version 14 will be installed automatically if you do not have already installed in your System.

If you already have the MSVC Package installed in your system, the Timestream ODBC driver installer will **not** 
upgrade the package for you. If you want to upgrade the package to the MSVC version that comes with the Timestream ODBC driver 
during driver installation, please uninstall the existing MSVC package first, and then run the driver installer.

For dependency uninstallation after Timestream ODBC driver is uninstalled, a **Modify Setup** window for 
Microsoft Visual C++ Redistributable (MSVC) Package could pop up with three options, "repair", "uninstall" and "close". 
If you choose "repair" the MSVC Package will be repaired or reinstalled and the Timestream ODBC driver will be removed. 
The "uninstall" will uninstall the MSVC Package. If you choose "close", the Timestream ODBC driver uninstallation will be rolled back. 



### Installation Over Pre-Existing Deployment Is Not Supported
The installer doesn't allow installation over pre-existing deployment. If a user would like to install a driver with newer or older 
release and there is already a Timestream ODBC driver installed on their machine, then the user will need to manually uninstall the 
existing driver before attempting to install a driver with a different version. 


## Next Steps

- [Set up the DSN](dsn-configuration.md)
