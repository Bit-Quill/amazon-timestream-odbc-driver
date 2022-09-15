# Troubleshooting Guide

## Topics
- [Logs](#logs)
- [PowerBI Desktop cannot load the Timestream ODBC driver library](#powerbi-desktop-cannot-load-the-timestream-odbc-driver-library)
- [Cannot connect on Linux using user DSN](#cannot-connect-on-linux-using-user-dsn)

## Logs

When troubleshooting, it can be helpful to view the logs so that you might be able 
to resolve the issue on your own or at least have more context to provide when seeking support. 
On Windows, the driver's logs will be written to the user's home directory (`%USERPROFILE%` or `%HOMEDRIVE%%HOMEPATH%`) by default.
On Mac/Linux/Unix, the default log path is also the user's home directory(`getpwuid()` or `$HOME`), whichever one is available.
On Windows, you may change the default path in the DSN configuration window.
In any platform, you may pass your log path / log level in the connection string.
The log path indicates the path to store the log file. The log file name has `timestream_odbc_YYYYMMDD.log` format, 
where `YYYYMMDD` (e.g., 20220225 <= Feb 25th, 2022)
is the timestamp at the first log message.
The keyword for log path is `logOutput` and the keyword for log level is `logLevel`. 

### Setting Logging Level and Location
There are the following levels of logging:

| Property Value | Description |
|--------|-------------|
| `1` (means ERROR) | Shows messages classified as ERROR.|
| `2` (means WARNING) | Shows messages classified as WARNING and ERROR.|
| `3` (means INFO) | Shows messages classified as INFO, WARNING and ERROR.|
| `4` (means DEBUG) | Shows messages classified as DEBUG, INFO, WARNING and ERROR.|
| `0` (means OFF) | No log messages displayed.|

| Property Name | Description | Platform | Default |
|--------|-------------|--------|---------------|
| `logLevel` | The log level for all sources/appenders. | All Platforms | `2` (means WARNING) |
| `logOutput` | The location for file logging. | Windows | `%USERPROFILE%`, or if not available, `%HOMEDRIVE%%HOMEPATH%` |
| `logOutput` | The location for file logging. | MacOS | `getpwuid()`, or if not available, `$HOME` |
| `logOutput` | The location for file logging. | Linux/Unix | `getpwuid()`, or if not available, `$HOME` |

To set these properties, use the connection string with the following format 
`<property-name>=<property-value>`. The user should **not** have a slash at the end of the log path. 

For example: (Note: The capitalization does not matter.)
- In Windows, append `logOutput="C:\Users\Name\Desktop\Timestream ODBC Driver";logLevel=4;` 
to your connection string to use log level DEBUG.
    * You can also set the log path and log level from the configuration window in the Microsoft ODBC Administrator. 
    * Click on the drop menu for setting the log level
    * Enter the desired log folder path in the field next to the label `Log Path`. The user needs to ensure that the directory mentioned in the log folder path does exist, or driver will ignore user's passed value and create the log file in the default log path.

- In MacOS/Linux/Unix, append `logOutput="~/path/to/log/file";logLevel=1;` to your connection string to use log level ERROR, or append
`logOutput` and `logLevel` as keywords in the ODBC manager. 

- If you just want to change the log level, append `logLevel=<desired-log-level>;` to your connection string.

## PowerBI Desktop cannot load the Timestream ODBC driver library

If you downloaded Power BI Desktop from the Microsoft Store, you may be unable to use the Amazon Timestream ODBC driver due to a loading issue. To address this, download Power BI Desktop from the [Download Center](https://www.microsoft.com/download/details.aspx?id=58494) instead of the Microsoft Store.

## Cannot connect on Linux using user DSN

If you have set the user DSN on Linux in `~/.odbc.ini`, but cannot connect via `isql`, try creating a system DSN in `/etc/odbc.ini` instead. Different flavors of Linux recognize different DSN files, and user DSNs might not be supported on some flavors.
