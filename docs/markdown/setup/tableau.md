# Tableau - Connecting to an Amazon Timestream ODBC data source

## Topics
- [Tableau on Windows](#windows)
- [Tableau on MacOS](#macos)

# Windows

## Prerequisites
Before you get started, [configure the Amazon Timestream ODBC driver DSN](windows-dsn-configuration.md) using the Windows ODBC Data Source Administrator. 

## Connecting to an ODBC data source

1. Open Tableau

2. Under section "To a Server", click "Other Databases (ODBC)" -> choose pre-defined DSN (e.g., Timestream DSN) -> press "Connect"

If a DSN window pops up at any time, just press the "Ok" button and continue

3. Click "Sign in"

4. You're now successfully connected on Tableau

# MacOS

## Prerequisites
Before you get started, [configure the Amazon Timestream ODBC driver DSN](macOS-dsn-configuration.md) by using the iODBC 
Driver Manager or editing `odbcinst.ini` file.

## Connecting to an ODBC data source

1. Open Tableau

2. Under section "To a Server", click "Other Databases (ODBC)"

![Tableau (Mac)](../images/mac-tableau.png)

3. Choose pre-defined DSN (e.g., Timestream-DSN) and click "Sign In"

![Tableau DSN (Mac)](../images/mac-tableau-odbc.png)

4. You're now successfully connected on Tableau
