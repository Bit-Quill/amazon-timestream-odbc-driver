# Microsoft Excel - Connecting to an Amazon Timestream ODBC data source

## Topics
- [Excel on Windows](#windows)
- [Excel on MacOS](#macos)

# Windows

## Prerequisites
Before you get started, [configure the Amazon Timestream ODBC driver DSN](windows-dsn-configuration.md) using the Windows ODBC Data Source Administrator. 

## Connecting to an ODBC data source

1. Open Excel

2. Go to "Data" tab

3. Go to "Get Data" -> Click "From Other Sources" -> Click "From ODBC"

![Excel ODBC](../images/excel_odbc.png)

4. Choose predefined DSN (e.g., Timestream DSN-test), click "OK"

![Excel DSN](../images/excel_dsn.png)

5. If this is the first time connecting to this DSN, select the authentication type and input your credentials when prompted. The authentication types available are:

    * Default or Custom: Select this authentication type when you don't specify any credentials when the DSN is configured with a username and password.
    * Windows: Select this authentication type if you want to connect using Windows authentication.
    * Database: Select this authentication type to use a username and password to access a data source with an ODBC driver. This is the default selection.

   Once you are done, click `Connect`.

![Excel Connect Popup](../images/excel_odbc_connect.png)

6. You're now successfully connected on Excel

# MacOS

## Prerequisites
Before you get started, [configure the Amazon Timestream ODBC driver DSN](macOS-dsn-configuration.md) by using the iODBC 
Driver Manager or editing `odbcinst.ini` file.

## Connecting to an ODBC data source

1. Open Excel

2. Create new excel sheet

3. Go to "Data" tab -> Click the arrow beside "Get Data (Power Query)" -> Click "From Database"

![Excel ODBC (Mac)](../images/mac_excel_odbc.png)

4. choose pre-defined DSN (e.g., Timestream-DSN) and click "Ok"

![Excel DSN (Mac)](../images/mac_excel_dsn.png)

5. You're now successfully connected on Excel
